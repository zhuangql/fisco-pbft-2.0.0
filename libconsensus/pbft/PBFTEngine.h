/*
 * @CopyRight:
 * FISCO-BCOS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * FISCO-BCOS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with FISCO-BCOS.  If not, see <http://www.gnu.org/licenses/>
 * (c) 2016-2018 fisco-dev contributors.
 */

/**
 * @brief : implementation of PBFT consensus
 * @file: PBFTEngine.h
 * @author: yujiechen
 * @date: 2018-09-28
 */
#pragma once
#include "Common.h"
#include "PBFTMsgCache.h"
#include "PBFTReqCache.h"
#include "PartiallyPBFTReqCache.h"
#include "TimeManager.h"
#include <libconsensus/ConsensusEngineBase.h>
#include <libdevcore/FileSystem.h>
#include <libdevcore/ThreadPool.h>
#include <libdevcore/concurrent_queue.h>
#include <libstorage/Storage.h>
#include <libsync/SyncStatus.h>
#include <sstream>

#include <libp2p/P2PMessageFactory.h>
#include <libp2p/P2PSession.h>
#include <libp2p/Service.h>

#include "PBFTMsgFactory.h"
#include <libstorage/BasicRocksDB.h>
#include <libsync/SyncStatus.h>

namespace dev
{
namespace consensus
{
enum CheckResult
{
    VALID = 0,
    INVALID = 1,
    FUTURE = 2
};
using PBFTMsgQueue = dev::concurrent_queue<PBFTMsgPacket::Ptr>;
class PBFTEngine : public ConsensusEngineBase, public std::enable_shared_from_this<PBFTEngine>
{
public:
    using Ptr = std::shared_ptr<PBFTEngine>;
    virtual ~PBFTEngine() { stop(); }
    PBFTEngine(std::shared_ptr<dev::p2p::P2PInterface> _service,
        std::shared_ptr<dev::txpool::TxPoolInterface> _txPool,
        std::shared_ptr<dev::blockchain::BlockChainInterface> _blockChain,
        std::shared_ptr<dev::sync::SyncInterface> _blockSync,
        std::shared_ptr<dev::blockverifier::BlockVerifierInterface> _blockVerifier,
        dev::PROTOCOL_ID const& _protocolId, KeyPair const& _keyPair,
        h512s const& _sealerList = h512s())
      : ConsensusEngineBase(_service, _txPool, _blockChain, _blockSync, _blockVerifier, _protocolId,
            _keyPair, _sealerList)
    {
        PBFTENGINE_LOG(INFO) << LOG_DESC("Register handler for PBFTEngine");

        m_broadCastCache = std::make_shared<PBFTBroadcastCache>();
        /// set thread name for PBFTEngine
        std::string threadName = "PBFT-" + std::to_string(m_groupId);
        setName(threadName);

        /// register checkSealerList to blockSync for check SealerList
        //检查什么？ 验证同步到的区块时候正确，通过sealist和commit消息
        m_blockSync->registerConsensusVerifyHandler(boost::bind(&PBFTEngine::checkBlock, this, _1));

        m_threadPool =
            std::make_shared<dev::ThreadPool>("pbftPool-" + std::to_string(m_groupId), 1);
        m_broacastTargetsFilter = boost::bind(&PBFTEngine::getIndexBySealer, this, _1);

        m_consensusSet = std::make_shared<std::set<dev::h512>>();

        m_messageHandler =
            std::make_shared<dev::ThreadPool>("PBFTMsg-" + std::to_string(m_groupId), 1);
        m_prepareWorker =
            std::make_shared<dev::ThreadPool>("PBFTWork-" + std::to_string(m_groupId), 1);

        m_destructorThread =
            std::make_shared<dev::ThreadPool>("PBFTAsync-" + std::to_string(m_groupId), 1);
        m_cachedForwardMsg =
            std::make_shared<std::map<dev::h256, std::pair<int64_t, PBFTMsgPacket::Ptr>>>();
    }

    void setBaseDir(std::string const& _path) { m_baseDir = _path; }

    std::string const& getBaseDir() { return m_baseDir; }

    /// set max block generation time
    inline void setEmptyBlockGenTime(unsigned const& _intervalBlockTime)
    {
        m_timeManager.m_emptyBlockGenTime = _intervalBlockTime;
    }
    /// get max block generation time
    inline unsigned const& getEmptyBlockGenTime() const
    {
        return m_timeManager.m_emptyBlockGenTime;
    }

    /// set mininum block generation time
    void setMinBlockGenerationTime(unsigned const& time)
    {
        if (time < m_timeManager.m_emptyBlockGenTime)
        {
            m_timeManager.m_minBlockGenTime = time;
        }
        else
        {
            m_timeManager.m_minBlockGenTime = (m_timeManager.m_emptyBlockGenTime - 1);
        }

        PBFTENGINE_LOG(INFO) << LOG_DESC("setMinBlockGenerationTime")
                             << LOG_KV("minBlockGenerationTime", m_timeManager.m_minBlockGenTime)
                             << LOG_KV("consensusTime", m_timeManager.m_emptyBlockGenTime);
    }

    void start() override;

    /// reach the minimum block generation time
    virtual bool reachMinBlockGenTime()
    {
        //是否达到最小区块生成时间
        /// since canHandleBlockForNextLeader has enforced the  next leader sealed block can't be
        /// handled before the current leader generate a new block, it's no need to add other
        /// conditions to enforce this striction
        return (utcSteadyTime() - m_timeManager.m_lastConsensusTime) >=
               m_timeManager.m_minBlockGenTime;
    }

    virtual bool reachBlockIntervalTime()
    {
        //是否达到空块产生时间
        /// since canHandleBlockForNextLeader has enforced the  next leader sealed block can't be
        /// handled before the current leader generate a new block, the conditions before can be
        /// deleted
        return (utcSteadyTime() - m_timeManager.m_lastConsensusTime) >=
               m_timeManager.m_emptyBlockGenTime;
    }

    /// in case of the next leader packeted the number of maxTransNum transactions before the last
    /// block is consensused
    /// when sealing for the next leader,  return true only if the last block has been consensused// 被执行？
    /// even if the maxTransNum condition has been meeted
    bool canHandleBlockForNextLeader()
    {
        /// get leader failed
        if (false == getLeader().first)
        {
            return false;
        }
        /// the case that only a node is both the leader and the next leader
        if (getLeader().second == nodeIdx())
        {
            return true;
        }
        if (m_notifyNextLeaderSeal && getNextLeader() == nodeIdx())
        {
            return false;
        }
        return true;
    }
    void rehandleCommitedPrepareCache(PrepareReq const& req);
    bool shouldSeal();
    /// broadcast prepare message
    bool generatePrepare(dev::eth::Block::Ptr _block);
    /// update the context of PBFT after commit a block into the block-chain
    void reportBlock(dev::eth::Block const& block) override;
    void onViewChange(std::function<void()> const& _f)
    {
        m_onViewChange = _f;
        m_notifyNextLeaderSeal = false;
    }
    void onNotifyNextLeaderReset(std::function<void(dev::h256Hash const& filter)> const& _f)
    {
        m_onNotifyNextLeaderReset = _f;
    }

    void onTimeout(std::function<void(uint64_t const& sealingTxNumber)> const& _f)
    {//动态调整区块大小
        m_onTimeout = _f;
    }
//动态调整区块大小
    void onCommitBlock(std::function<void(uint64_t const& blockNumber,
            uint64_t const& sealingTxNumber, unsigned const& changeCycle)> const& _f)
    {
        m_onCommitBlock = _f;
    }

    bool inline shouldReset(dev::eth::Block const& block)
    {
        return block.getTransactionSize() == 0 && m_omitEmptyBlock;
    }
    const std::string consensusStatus() override;
    void setOmitEmptyBlock(bool setter) { m_omitEmptyBlock = setter; }

    void setMaxTTL(uint8_t const& ttl) { maxTTL = ttl; }
    //每出一个块,view归0，这样下一个leader不受viewChange影响
    inline IDXTYPE getNextLeader() const { return (m_highestBlock.number() + 1) % m_nodeNum; }

    virtual std::pair<bool, IDXTYPE> getLeader() const
    {
        if (m_cfgErr || m_leaderFailed || m_highestBlock.sealer() == Invalid256 || m_nodeNum == 0)
        {
            return std::make_pair(false, MAXIDX);
        }
        return std::make_pair(true, (m_view + m_highestBlock.number()) % m_nodeNum);
    }

    uint64_t sealingTxNumber() const { return m_sealingNumber; }

    VIEWTYPE view() const override { return m_view; }
    VIEWTYPE toView() const override { return m_toView; }
    void resetConfig() override;
    void setEnableTTLOptimize(bool const& _enableTTLOptimize)
    {
        m_enableTTLOptimize = _enableTTLOptimize;
    }

    void setEnablePrepareWithTxsHash(bool const& _enablePrepareWithTxsHash)
    {
        m_enablePrepareWithTxsHash = _enablePrepareWithTxsHash;
    }

    void stop() override;

    virtual void createPBFTReqCache();

    /// get the index of specified sealer according to its node id
    /// @param nodeId: the node id of the sealer
    /// @return : 1. >0: the index of the sealer
    ///           2. equal to -1: the node is not a sealer(not exists in sealer list)
    inline ssize_t getIndexBySealer(dev::network::NodeID const& nodeId)
    {
        ReadGuard l(m_sealerListMutex);
        ssize_t index = -1;
        for (size_t i = 0; i < m_sealerList.size(); ++i)
        {
            if (m_sealerList[i] == nodeId)
            {
                index = i;
                break;
            }
        }
        return index;
    }

protected:
    virtual void resetConsensusTimeout();

    virtual bool locatedInChosedConsensensusNodes() const { return m_idx != MAXIDX; }
    virtual void addRawPrepare(PrepareReq::Ptr _prepareReq);
    void reportBlockWithoutLock(dev::eth::Block const& block);
    void workLoop() override;
    void handleFutureBlock();
    void collectGarbage();
    void checkTimeout();
    bool getNodeIDByIndex(dev::network::NodeID& nodeId, const IDXTYPE& idx) const;
    virtual dev::h512 selectNodeToRequestMissedTxs(PrepareReq::Ptr _prepareReq);

    void checkBlockValid(dev::eth::Block const& block) override
    {
        ConsensusEngineBase::checkBlockValid(block);
        checkSealerList(block);
    }

    virtual void checkTransactionsValid(dev::eth::Block::Ptr, PrepareReq::Ptr) {}

    bool needOmit(Sealing const& sealing);

    void getAllNodesViewStatus(Json::Value& status);

    // broadcast given messages to all-peers with cache-filter and specified filter
    virtual bool broadcastMsg(unsigned const& _packetType, PBFTMsg const& _pbftMsg,
        bytesConstRef _data, PACKET_TYPE const& _p2pPacketType,
        std::unordered_set<dev::network::NodeID> const& _filter, unsigned const& _ttl,
        std::function<ssize_t(dev::network::NodeID const&)> const& _filterFunction);

    bool broadcastMsg(unsigned const& _packetType, PBFTMsg const& _pbftMsg, bytesConstRef _data,
        PACKET_TYPE const& _p2pPacketType = 0,
        std::unordered_set<dev::network::NodeID> const& _filter =
            std::unordered_set<dev::network::NodeID>(),
        unsigned const& _ttl = 0)
    {
        return broadcastMsg(
            _packetType, _pbftMsg, _data, _p2pPacketType, _filter, _ttl, m_broacastTargetsFilter);
    }

    void sendViewChangeMsg(dev::network::NodeID const& nodeId);
    bool sendMsg(dev::network::NodeID const& nodeId, unsigned const& packetType,
        std::string const& key, bytesConstRef data, unsigned const& ttl = 1,
        std::shared_ptr<dev::h512s> forwardNodes = nullptr);
    /// 1. generate and broadcast signReq according to given prepareReq
    /// 2. add the generated signReq into the cache
    bool broadcastSignReq(PrepareReq const& req);

    /// broadcast commit message
    bool broadcastCommitReq(PrepareReq const& req);
    /// broadcast view change message
    bool shouldBroadcastViewChange();
    bool broadcastViewChangeReq();
    /// handler called when receiving data from the network
    void pushValidPBFTMsgIntoQueue(dev::p2p::NetworkException exception,
        std::shared_ptr<dev::p2p::P2PSession> session, dev::p2p::P2PMessage::Ptr message,
        std::function<void(PBFTMsgPacket::Ptr)> const& _f);

    virtual void onRecvPBFTMessage(dev::p2p::NetworkException _exception,
        std::shared_ptr<dev::p2p::P2PSession> _session, dev::p2p::P2PMessage::Ptr _message);

    bool handlePrepareMsg(PrepareReq::Ptr prepare_req, std::string const& endpoint = "self");
    /// handler prepare messages
    bool handlePrepareMsg(PrepareReq::Ptr prepareReq, PBFTMsgPacket const& pbftMsg);

    /// 1. decode the network-received PBFTMsgPacket to signReq
    /// 2. check the validation of the signReq
    /// add the signReq to the cache and
    /// heck the size of the collected signReq is over 2/3 or not
    bool handleSignMsg(SignReq::Ptr signReq, PBFTMsgPacket const& pbftMsg);
    bool handleCommitMsg(CommitReq::Ptr commitReq, PBFTMsgPacket const& pbftMsg);
    bool handleViewChangeMsg(ViewChangeReq::Ptr viewChangeReq, PBFTMsgPacket const& pbftMsg);
    void handleMsg(PBFTMsgPacket::Ptr pbftMsg);
    void catchupView(ViewChangeReq const& req, std::ostringstream& oss);
    void checkAndCommit();

    /// if collect >= 2/3 SignReq and CommitReq, then callback this function to commit block
    void checkAndSave();
    void checkAndChangeView();


    void initPBFTEnv(unsigned _view_timeout);
    virtual void initBackupDB();
    void reloadMsg(std::string const& _key, PBFTMsg* _msg);
    void backupMsg(std::string const& _key, std::shared_ptr<bytes> _msg);
    inline std::string getBackupMsgPath() { return m_baseDir + "/" + c_backupMsgDirName; }

    bool checkSign(PBFTMsg const& req) const;
    bool checkSign(
        IDXTYPE const& _idx, dev::h256 const& _hash, std::vector<unsigned char> const& _sig);

    inline bool broadcastFilter(
        dev::network::NodeID const& nodeId, unsigned const& packetType, std::string const& key)
    {
        return m_broadCastCache->keyExists(nodeId, packetType, key);
    }

    /**
     * @brief: insert specified key into the cache of broadcast
     *         used to filter the broadcasted message(in case of too-many repeated broadcast
     * messages)
     * @param nodeId: the node id of the message broadcasted to
     * @param packetType: the packet type of the broadcast-message
     * @param key: the key of the broadcast-message, is the signature of the broadcast-message in
     * common
     */
    inline void broadcastMark(
        dev::network::NodeID const& nodeId, unsigned const& packetType, std::string const& key)
    {
        /// in case of useless insert
        if (m_broadCastCache->keyExists(nodeId, packetType, key))
            return;
        m_broadCastCache->insertKey(nodeId, packetType, key);
    }
    inline void clearMask() { m_broadCastCache->clearAll(); }

    /// get the node id of specified sealer according to its index
    /// @param index: the index of the node
    /// @return h512(): the node is not in the sealer list
    /// @return node id: the node id of the node
    virtual dev::network::NodeID getSealerByIndex(size_t const& index) const
    {//NodeID是64位byte数组，配置文件中应该是16进制字符串，是128个，16进制两个数占8位，所以一共64位
        ReadGuard l(m_sealerListMutex);
        if (index < m_sealerList.size())
            return m_sealerList[index];//sealerList中，所有节点的配置文件的节点顺序需要一致，用vector中的索引标识节点index
        return dev::network::NodeID();
    }

    virtual PBFTMsgPacket::Ptr createPBFTMsgPacket(bytesConstRef data,
        PACKET_TYPE const& packetType, unsigned const& ttl,
        std::shared_ptr<dev::h512s> _forwardNodes);

    /// trans data into message
    virtual dev::p2p::P2PMessage::Ptr transDataToMessage(bytesConstRef _data,
        PACKET_TYPE const& _packetType, unsigned const& _ttl,
        std::shared_ptr<dev::h512s> _forwardNodes = nullptr);

    /**
     * @brief : the message received from the network is valid or not?
     *      invalid cases: 1. received data is empty
     *                     2. the message is not sended by sealers
     *                     3. the message is not receivied by sealers
     *                     4. the message is sended by the node-self
     * @param message : message constructed from data received from the network
     * @param session : the session related to the network data(can get informations about the
     * sender)
     * @return true : the network-received message is valid
     * @return false: the network-received message is invalid
     *///好像没什么大用处
    bool isValidReq(dev::p2p::P2PMessage::Ptr message,
        std::shared_ptr<dev::p2p::P2PSession> session, ssize_t& peerIndex) override
    {
        /// check message size
        //要看p2pmessage类 zhuangql
        if (message->buffer()->size() <= 0)                                       //1。消息非空
            return false;
        /// check whether in the sealer list
        //？？？session的id是发送者的id还是转发者的id 是转发着发的
        peerIndex = getIndexBySealer(session->nodeID());       //2。消息是sealers中节点转发的  则消息有效
        if (peerIndex < 0)
        {
            PBFTENGINE_LOG(TRACE) << LOG_DESC(
                "isValidReq: Recv PBFT msg from unkown peer:" + session->nodeID().abridged());
            return false;
        }
        /// check whether this node is in the sealer list
        dev::network::NodeID node_id;
        bool is_sealer = getNodeIDByIndex(node_id, nodeIdx());
        if (!is_sealer || session->nodeID() == node_id)//3。消息不是sealers接收 4。消息是自己发送的 则消息无效
            return false;
        return true;
    }

    /// check the specified prepareReq is valid or not
    CheckResult isValidPrepare(PrepareReq const& req, std::ostringstream& oss) const;

    /**
     * @brief: common check process when handle SignReq and CommitReq
     *         1. the request should be existed in prepare cache,
     *            if the request is the future request, should add it to the prepare cache
     *         2. the sealer of the request shouldn't be the node-self
     *         3. the view of the request must be equal to the view of the prepare cache
     *         4. the signature of the request must be valid
     * @tparam T: the type of the request
     * @param req: the request should be checked
     * @param oss: information to debug
     * @return CheckResult:
     *  1. CheckResult::FUTURE: the request is the future req;
     *  2. CheckResult::INVALID: the request is invalid
     *  3. CheckResult::VALID: the request is valid
     */
    template <class T>
    inline CheckResult checkReq(T& req, std::ostringstream& oss) const
    {
        if (isSyncingHigherBlock(req))
        {
            PBFTENGINE_LOG(DEBUG) << LOG_DESC("checkReq: Is Syncing higher number")
                                  << LOG_KV("ReqNumber", req.height)
                                  << LOG_KV(
                                         "syncingNumber", m_blockSync->status().knownHighestNumber)
                                  << LOG_KV("INFO", oss.str());
            return CheckResult::INVALID;
        }

        //未来块的一种特殊情况：leader作恶发送了mvn和m’vn，f个可信节点认可m’vn，f+1个可信节点认可mvn
        //则f个可信节点中的preparecache是m’vn，并不会达成prepared阶段，但会收到2f+1个mvn的signReq消息
        //此时f个可信节点当其为未来块忽略req消息，最后f+1个可信节点上链m后，f个可信节点同步区块
        //另外一种：f个可信结点收到的m‘是无效的，当最后收到了2f+1个m的commit消息
        if (m_reqCache->prepareCache().block_hash != req.block_hash)
        {//可能当前高度的prepare消息来了，当pre-prepare区块还没来，此时的prepareCache缓存是上一高度的区块
            PBFTENGINE_LOG(TRACE) << LOG_DESC("checkReq: sign or commit Not exist in prepare cache")
                                  << LOG_KV("prepHash",
                                         m_reqCache->prepareCache().block_hash.abridged())
                                  << LOG_KV("hash", req.block_hash.abridged())
                                  << LOG_KV("INFO", oss.str());
            /// is future ?
            bool is_future = isFutureBlock(req);
            if (is_future)
            {
                req.isFuture = true;
                req.signChecked = false;
                PBFTENGINE_LOG(INFO)
                    << LOG_DESC("checkReq: Recv future request")
                    << LOG_KV("prepHash", m_reqCache->prepareCache().block_hash.abridged())
                    << LOG_KV("INFO", oss.str());
                return CheckResult::FUTURE;
            }
            return CheckResult::INVALID;
        }
        /// check the sealer of this request
        if (req.idx == nodeIdx())
        {
            PBFTENGINE_LOG(TRACE) << LOG_DESC("checkReq: Recv own req")
                                  << LOG_KV("INFO", oss.str());
            return CheckResult::INVALID;
        }
        /// check view
        if (m_reqCache->prepareCache().view != req.view)
        {
            PBFTENGINE_LOG(TRACE) << LOG_DESC("checkReq: Recv req with unconsistent view")
                                  << LOG_KV("prepView", m_reqCache->prepareCache().view)
                                  << LOG_KV("view", req.view) << LOG_KV("INFO", oss.str());
            return CheckResult::INVALID;
        }
        if (!checkSign(req))
        {
            PBFTENGINE_LOG(TRACE) << LOG_DESC("checkReq:  invalid sign")
                                  << LOG_KV("INFO", oss.str());
            return CheckResult::INVALID;
        }
        return CheckResult::VALID;
    }

    CheckResult isValidSignReq(SignReq::Ptr req, std::ostringstream& oss) const;
    CheckResult isValidCommitReq(CommitReq::Ptr req, std::ostringstream& oss) const;
    bool isValidViewChangeReq(
        ViewChangeReq const& req, IDXTYPE const& source, std::ostringstream& oss);

    template <class T>
    inline bool hasConsensused(T const& req) const
    {
        if (req.height < m_consensusBlockNumber ||
            (req.height == m_consensusBlockNumber && req.view < m_view))
        {
            return true;
        }
        return false;
    }

    /// in case of con-current execution of block //同步优先吗？ zhuangql
    template <class T>
    inline bool isSyncingHigherBlock(T const& req) const
    {
        // when the node falling far behind, will not handle the received PBFT message
        //节点同步模块已知的区块高度落后最新高度20，不接收消息专心同步
        if (m_blockSync->blockNumberFarBehind())
        {
            return true;
        }
        //正在同步&&req是已经上链的区块，req无效
        if (m_blockSync->isSyncing() && req.height <= m_blockSync->status().knownHighestNumber)
        {
            return true;
        }
        return false;
    }
    /**
     * @brief : decide the sign or commit request is the future request or not
     *          1. the block number is no smalller than the current consensused block number
     *          2. or the view is no smaller than the current consensused block number
     */
    template <typename T>
    inline bool isFutureBlock(T const& req) const
    {
        /// to ensure that the signReq can reach to consensus even if the view has been changed
        if (req.height >= m_consensusBlockNumber || req.view > m_view) //有点问题？什么情况下第二个条件能走到 zhuangql
        //(req.height > m_consensusBlockNumber || 
        //(req.height == m_consensusBlockNumber && req.view >= m_view))
        {
            return true;
        }
        return false;
    }//防止没收到当前高度区块但先收到了当前高度的消息，消息也是有效的，是未来块消息

    template <typename T>
    inline bool isFuturePrepare(T const& req) const
    {
        if (req.height > m_consensusBlockNumber ||
            (req.height == m_consensusBlockNumber && req.view > m_view))
        {
            return true;
        }
        return false;
    }

    inline bool isHashSavedAfterCommit(PrepareReq const& req) const
    {
        if (req.height == m_reqCache->committedPrepareCache().height && //若高度不等，则req是未来块消息，返回true
            //若高度相等，说明当前高度的 之前共识轮或当前轮的block_hash已经达到commit阶段，block_hash不等，不接受此消息
            //说明viewChange后下一个leader要沿用之前共识轮达到commit阶段的区块。block_hash相等的情况在isExistPrepare就会被return
            req.block_hash != m_reqCache->committedPrepareCache().block_hash)
        {
            PBFTENGINE_LOG(DEBUG)
                << LOG_DESC("isHashSavedAfterCommit: hasn't been cached after commit")
                << LOG_KV("height", req.height)
                << LOG_KV("cacheHeight", m_reqCache->committedPrepareCache().height)
                << LOG_KV("hash", req.block_hash.abridged())
                << LOG_KV("cacheHash", m_reqCache->committedPrepareCache().block_hash.abridged());
            return false;
        }
        return true;
    }

    inline bool isValidLeader(PrepareReq const& req) const
    {
        auto leader = getLeader();
        /// get leader failed or this prepareReq is not broadcasted from leader
        if (!leader.first || req.idx != leader.second)
        {
            return false;
        }

        return true;
    }

    void checkSealerList(dev::eth::Block const& block);
    /// check block
    bool checkBlock(dev::eth::Block const& block);
    void execBlock(Sealing& sealing, PrepareReq::Ptr _req, std::ostringstream& oss);
    void changeViewForFastViewChange()
    {
        m_timeManager.changeView();
        m_fastViewChange = true;
        m_signalled.notify_all();
    }
    void notifySealing(dev::eth::Block const& block);
    /// to ensure at least 100MB available disk space
    virtual bool isDiskSpaceEnough(std::string const& path)
    {
        return boost::filesystem::space(path).available > 1024 * 1024 * 100;
    }

    void updateViewMap(IDXTYPE const& idx, VIEWTYPE const& view)
    {
        WriteGuard l(x_viewMap);
        m_viewMap[idx] = view;
    }

    void waitSignal();

    template <class T>
    inline bool decodePBFTMsgPacket(std::shared_ptr<T> _req,
        std::shared_ptr<dev::p2p::P2PMessage> _message,
        std::shared_ptr<dev::p2p::P2PSession> _session)
    {
        ssize_t peerIndex = 0;
        bool valid = isValidReq(_message, _session, peerIndex);
        if (valid)
        {
            try
            {
                _req->decode(ref(*(_message->buffer())));
            }
            catch (std::exception& e)
            {
                PBFTENGINE_LOG(DEBUG) << "[decodeToRequests] Invalid network-received packet";
                return false;
            }
            _req->setOtherField(peerIndex, _session->nodeID(),
                boost::lexical_cast<std::string>(_session->session()->nodeIPEndpoint()));
        }
        return valid;
    }

    // ttl opitimize related
    virtual bool needForwardMsg(
        bool const& _valid, PBFTMsgPacket::Ptr _pbftMsgPacket, PBFTMsg const& _pbftMsg);
    // forward message
    virtual void forwardMsg(PBFTMsgPacket::Ptr _pbftMsgPacket, PBFTMsg const& _pbftMsg);
    void forwardMsgByTTL(
        PBFTMsgPacket::Ptr _pbftMsgPacket, PBFTMsg const& _pbftMsg, bytesConstRef _data);
    void forwardMsgByNodeInfo(
        std::string const& _key, PBFTMsgPacket::Ptr _pbftMsgPacket, bytesConstRef _data);

    virtual void createPBFTMsgFactory();

    virtual void broadcastMsg(dev::h512s const& _targetNodes, bytesConstRef _data,
        unsigned const& _packetType, unsigned const& _ttl, PACKET_TYPE const& _p2pPacketType,
        PBFTMsg const& _pbftMsg);
    std::shared_ptr<dev::h512s> getForwardNodes(bool const& _printLog = false);


    // BIP 152 related logic
    virtual void handleP2PMessage(dev::p2p::NetworkException _exception,
        std::shared_ptr<dev::p2p::P2PSession> _session, dev::p2p::P2PMessage::Ptr _message);


    virtual PrepareReq::Ptr constructPrepareReq(dev::eth::Block::Ptr _block);
    virtual void sendPrepareMsgFromLeader(PrepareReq::Ptr _prepareReq, bytesConstRef _data,
        dev::PACKET_TYPE const& _p2pPacketType = 0);

    virtual dev::p2p::P2PMessage::Ptr toP2PMessage(
        std::shared_ptr<bytes> _data, PACKET_TYPE const& _packetType);

    bool handleReceivedPartiallyPrepare(std::shared_ptr<dev::p2p::P2PSession> _session,
        dev::p2p::P2PMessage::Ptr _message, std::function<void(PBFTMsgPacket::Ptr)> const& _f);

    virtual bool handlePartiallyPrepare(
        std::shared_ptr<dev::p2p::P2PSession> _session, dev::p2p::P2PMessage::Ptr _message);

    bool handlePartiallyPrepare(PrepareReq::Ptr _prepareReq);

    bool execPrepareAndGenerateSignMsg(PrepareReq::Ptr _prepareReq, std::ostringstream& _oss);
    void forwardPrepareMsg(PBFTMsgPacket::Ptr _pbftMsgPacket, PrepareReq::Ptr prepareReq);
    void onReceiveGetMissedTxsRequest(
        std::shared_ptr<dev::p2p::P2PSession> _session, dev::p2p::P2PMessage::Ptr _message);
    void onReceiveMissedTxsResponse(
        std::shared_ptr<dev::p2p::P2PSession> _session, dev::p2p::P2PMessage::Ptr _message);
    void clearInvalidCachedForwardMsg();

    void clearPreRawPrepare();

    bool requestMissedTxs(PrepareReq::Ptr _prepareReq);

protected:
    std::atomic<VIEWTYPE> m_view = {0};
    std::atomic<VIEWTYPE> m_toView = {0};
    std::string m_baseDir;
    std::atomic_bool m_leaderFailed = {false};
    //执行prepare区块时设置为true
    std::atomic_bool m_notifyNextLeaderSeal = {false};

    // backup msg
    dev::storage::BasicRocksDB::Ptr m_backupDB = nullptr;

    /// static vars
    static const std::string c_backupKeyCommitted;
    static const std::string c_backupMsgDirName;
    static const unsigned c_PopWaitSeconds = 5;
//插入指定的key到cache，用来过滤已经被广播的消息，防止有太多重复的broadcast message
//在filter list里的节点，标记消息已广播， cpp  134行（cache中的消息对应的sign已经有这条消息了，所以不用广播了）
    std::shared_ptr<PBFTBroadcastCache> m_broadCastCache;
    //存储pbft4种请求投票
    std::shared_ptr<PBFTReqCache> m_reqCache;
    TimeManager m_timeManager;
    PBFTMsgQueue m_msgQueue;
    mutable Mutex m_mutex;

    boost::condition_variable m_signalled;
    boost::mutex x_signalled;


    std::function<void()> m_onViewChange = nullptr;
    std::function<void(dev::h256Hash const& filter)> m_onNotifyNextLeaderReset = nullptr;
    //动态调整区块大小
    std::function<void(uint64_t const& sealingTxNumber)> m_onTimeout = nullptr;
    //动态调整区块大小
    std::function<void(
        uint64_t const& blockNumber, uint64_t const& sealingTxNumber, unsigned const& changeCycle)>
        m_onCommitBlock = nullptr;

    std::function<ssize_t(dev::network::NodeID const&)> m_broacastTargetsFilter = nullptr;

    /// for output time-out caused viewchange
    /// m_fastViewChange is false: output viewchangeWarning to indicate PBFT consensus timeout
    std::atomic_bool m_fastViewChange = {false};

    uint8_t maxTTL = MAXTTL;

    /// map between nodeIdx to view
    mutable SharedMutex x_viewMap;
    std::map<IDXTYPE, VIEWTYPE> m_viewMap;

    std::atomic<uint64_t> m_sealingNumber = {0};

    // the thread pool is used to execute the async-function
    dev::ThreadPool::Ptr m_threadPool;
    //start()时初始化，pbftMsg消息工厂
    PBFTMsgFactory::Ptr m_pbftMsgFactory = nullptr;
    // ttl-optimize related logic
    bool m_enableTTLOptimize = false;    //优化了PBFT消息转发机制，保证网络断连场景下PBFT消息包能尽量到达每个共识节点的同时，降低网络中冗余的PBFT消息包
    mutable SharedMutex x_consensusSet;
    std::shared_ptr<std::set<dev::h512>> m_consensusSet;

    // bip 152 related logic
    PartiallyPBFTReqCache::Ptr m_partiallyPrepareCache = nullptr;
    std::shared_ptr<std::map<dev::h256, std::pair<int64_t, PBFTMsgPacket::Ptr>>> m_cachedForwardMsg;
    dev::ThreadPool::Ptr m_prepareWorker;
    dev::ThreadPool::Ptr m_messageHandler;

    // Make object destructive overhead asynchronous
    dev::ThreadPool::Ptr m_destructorThread;
    bool m_enablePrepareWithTxsHash = false; //关闭Prepare包结构优化，Prepare消息包内区块包含全量的交易
};
}  // namespace consensus
}  // namespace dev
