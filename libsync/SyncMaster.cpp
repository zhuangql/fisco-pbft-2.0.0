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
 * @brief : Sync implementation
 * @author: jimmyshi
 * @date: 2018-10-15
 */

#include "SyncMaster.h"
#include <json/json.h>
#include <libblockchain/BlockChainInterface.h>

using namespace std;
using namespace dev;
using namespace dev::eth;
using namespace dev::sync;
using namespace dev::p2p;
using namespace dev::blockchain;
using namespace dev::txpool;
using namespace dev::blockverifier;

static unsigned const c_maxSendTransactions = 1000;

void SyncMaster::printSyncInfo()
{
    auto pendingSize = m_txPool->pendingSize();
    NodeIDs peers = m_syncStatus->peers();
    std::string peer_str;
    for (auto& peer : peers)
    {
        peer_str += peer.abridged() + "/";
    }
    SYNC_LOG(TRACE) << "\n[Sync Info] --------------------------------------------\n"
                    << "            IsSyncing:    " << isSyncing() << "\n"
                    << "            Block number: " << m_blockChain->number() << "\n"
                    << "            Block hash:   "
                    << m_blockChain->numberHash(m_blockChain->number()) << "\n"
                    << "            Genesis hash: " << m_syncStatus->genesisHash.abridged() << "\n"
                    << "            TxPool size:  " << pendingSize << "\n"
                    << "            Peers size:   " << m_syncStatus->peers().size() << "\n"
                    << "[Peer Info] --------------------------------------------\n"
                    << "    Host: " << m_nodeId.abridged() << "\n"
                    << "    Peer: " << peer_str << "\n"
                    << "            --------------------------------------------";
}

SyncStatus SyncMaster::status() const
{
    SyncStatus res;
    res.state = m_syncStatus->state;
    res.protocolId = m_protocolId;
    res.currentBlockNumber = m_blockChain->number();
    res.knownHighestNumber = m_syncStatus->knownHighestNumber;
    res.knownLatestHash = m_syncStatus->knownLatestHash;
    return res;
}

string const SyncMaster::syncInfo() const
{
    Json::Value syncInfo;
    syncInfo["isSyncing"] = isSyncing();
    syncInfo["protocolId"] = m_protocolId;
    syncInfo["genesisHash"] = toHexPrefixed(m_syncStatus->genesisHash);
    syncInfo["nodeId"] = toHex(m_nodeId);

    int64_t currentNumber = m_blockChain->number();
    syncInfo["blockNumber"] = currentNumber;
    syncInfo["latestHash"] = toHexPrefixed(m_blockChain->numberHash(currentNumber));
    syncInfo["knownHighestNumber"] = m_syncStatus->knownHighestNumber;
    syncInfo["knownLatestHash"] = toHex(m_syncStatus->knownLatestHash);
    syncInfo["txPoolSize"] = std::to_string(m_txPool->pendingSize());

    Json::Value peersInfo(Json::arrayValue);
    m_syncStatus->foreachPeer([&](shared_ptr<SyncPeerStatus> _p) {
        Json::Value info;
        info["nodeId"] = toHex(_p->nodeId);
        info["genesisHash"] = toHexPrefixed(_p->genesisHash);
        info["blockNumber"] = _p->number;
        info["latestHash"] = toHexPrefixed(_p->latestHash);
        peersInfo.append(info);
        return true;
    });

    syncInfo["peers"] = peersInfo;
    Json::FastWriter fastWriter;
    std::string statusStr = fastWriter.write(syncInfo);
    return statusStr;
}

void SyncMaster::start()
{
    if (!fp_isConsensusOk)
    {
        SYNC_LOG(ERROR) << LOG_DESC("Consensus verify handler is not set");
        BOOST_THROW_EXCEPTION(SyncVerifyHandlerNotSet());
    }

    startWorking();
}

void SyncMaster::stop()
{
    doneWorking();
    stopWorking();
    // will not restart worker, so terminate it
    terminate();
}

void SyncMaster::doWork()
{
    auto start_time = utcTime();
    auto record_time = utcTime();
    // Debug print
    if (isSyncing())
        printSyncInfo();
    auto printSyncInfo_time_cost = utcTime() - record_time;
    record_time = utcTime();

    // Always do
    maintainPeersConnection();//维护（链接）同步节点列表
    auto maintainPeersConnection_time_cost = utcTime() - record_time;
    record_time = utcTime();

    maintainDownloadingQueueBuffer();//downloading:把区块缓存 转入 队列存放    idle:清空
    auto maintainDownloadingQueueBuffer_time_cost = utcTime() - record_time;
    record_time = utcTime();

    maintainPeersStatus();//若符合条件，则变成downloading，发送下载请求
    auto maintainPeersStatus_time_cost = utcTime() - record_time;
    record_time = utcTime();

    maintainDownloadingTransactions();
    auto maintainDownloadingTransactions_time_cost = utcTime() - record_time;
    record_time = utcTime();
    maintainBlocks();//发送 同步状态消息包       新区块上链发/5s发一次
    auto maintainBlocks_time_cost = utcTime() - record_time;
    record_time = utcTime();

    auto maintainTransactions_time_cost = 0;
    auto maintainBlockRequest_time_cost = 0;
    // Idle do
    if (!isSyncing())
    {
        // cout << "SyncMaster " << m_protocolId << " doWork()" << endl;
        if (m_needMaintainTransactions && m_newTransactions)
        {
            m_newTransactions = false;
            maintainTransactions();
        }
        maintainTransactions_time_cost = utcTime() - record_time;
        record_time = utcTime();

        maintainBlockRequest();
        maintainBlockRequest_time_cost = utcTime() - record_time;
        record_time = utcTime();
    }

    auto maintainDownloadingQueue_time_cost = 0;
    // Not Idle do
    if (isSyncing())
    {
        if (m_syncStatus->state == SyncState::Downloading)
        {
            bool finished = maintainDownloadingQueue();
            if (finished)
                noteDownloadingFinish();
        }
        maintainDownloadingQueue_time_cost = utcTime() - record_time;
    }

    SYNC_LOG(TRACE) << LOG_BADGE("Record") << LOG_DESC("Sync loop time record")
                    << LOG_KV("printSyncInfoTimeCost", printSyncInfo_time_cost)
                    << LOG_KV("maintainPeersConnectionTimeCost", maintainPeersConnection_time_cost)
                    << LOG_KV("maintainDownloadingQueueBufferTimeCost",
                           maintainDownloadingQueueBuffer_time_cost)
                    << LOG_KV("maintainPeersStatusTimeCost", maintainPeersStatus_time_cost)
                    << LOG_KV("maintainBlocksTimeCost", maintainBlocks_time_cost)
                    << LOG_KV("maintainDownloadingTransactionsTimeCost",
                           maintainDownloadingTransactions_time_cost)
                    << LOG_KV("maintainTransactionsTimeCost", maintainTransactions_time_cost)
                    << LOG_KV("maintainBlockRequestTimeCost", maintainBlockRequest_time_cost)
                    << LOG_KV(
                           "maintainDownloadingQueueTimeCost", maintainDownloadingQueue_time_cost)
                    << LOG_KV("syncTotalTimeCost", utcTime() - start_time);
}

void SyncMaster::workLoop()
{
    while (workerState() == WorkerState::Started)
    {
        doWork();
        if (idleWaitMs())
        {
            std::unique_lock<std::mutex> l(x_signalled);
            m_signalled.wait_for(l, std::chrono::milliseconds(idleWaitMs()));
        }
    }
}


void SyncMaster::noteSealingBlockNumber(int64_t _number)
{
    WriteGuard l(x_currentSealingNumber);
    m_currentSealingNumber = _number;
    m_signalled.notify_all();
}

bool SyncMaster::isSyncing() const
{
    return m_syncStatus->state != SyncState::Idle;
}

// is my number is far smaller than max block number of this block chain
bool SyncMaster::isFarSyncing() const
{
    int64_t currentNumber = m_blockChain->number();
    return m_syncStatus->knownHighestNumber - currentNumber > 10;
}

void SyncMaster::maintainTransactions()
{
    unordered_map<NodeID, std::vector<size_t>> peerTransactions;

    auto ts = m_txPool->topTransactionsCondition(c_maxSendTransactions, m_nodeId);
    auto txSize = ts.size();
    auto pendingSize = m_txPool->pendingSize();

    SYNC_LOG(TRACE) << LOG_BADGE("Tx") << LOG_DESC("Transaction need to send ")
                    << LOG_KV("txs", txSize) << LOG_KV("totalTxs", pendingSize);
    UpgradableGuard l(m_txPool->xtransactionKnownBy());
    for (size_t i = 0; i < ts.size(); ++i)
    {
        auto const& t = ts[i];
        NodeIDs peers;
        unsigned _percent = m_txPool->isTransactionKnownBySomeone(t.sha3()) ? 25 : 100;

        peers = m_syncStatus->randomSelection(_percent, [&](std::shared_ptr<SyncPeerStatus> _p) {
            bool unsent = !m_txPool->isTransactionKnownBy(t.sha3(), m_nodeId);
            bool isSealer = _p->isSealer;
            return isSealer && unsent && !m_txPool->isTransactionKnownBy(t.sha3(), _p->nodeId);
        });
        if (0 == peers.size())
            return;
        UpgradeGuard ul(l);
        m_txPool->setTransactionIsKnownBy(t.sha3(), m_nodeId);
        for (auto const& p : peers)
        {
            peerTransactions[p].push_back(i);
            m_txPool->setTransactionIsKnownBy(t.sha3(), p);
        }
    }

    m_syncStatus->foreachPeerRandom([&](shared_ptr<SyncPeerStatus> _p) {
        std::vector<bytes> txRLPs;
        unsigned txsSize = peerTransactions[_p->nodeId].size();
        if (0 == txsSize)
            return true;  // No need to send

        for (auto const& i : peerTransactions[_p->nodeId])
            txRLPs.emplace_back(ts[i].rlp(WithSignature));

        SyncTransactionsPacket packet;
        packet.encode(txRLPs);

        auto msg = packet.toMessage(m_protocolId);
        m_service->asyncSendMessageByNodeID(_p->nodeId, msg, CallbackFuncWithSession(), Options());

        SYNC_LOG(DEBUG) << LOG_BADGE("Tx") << LOG_DESC("Send transaction to peer")
                        << LOG_KV("txNum", int(txsSize))
                        << LOG_KV("toNodeId", _p->nodeId.abridged())
                        << LOG_KV("messageSize(B)", msg->buffer()->size());
        return true;
    });
}

void SyncMaster::maintainDownloadingTransactions()
{
    m_txQueue->pop2TxPool(m_txPool);
}

void SyncMaster::maintainBlocks()
{
    if (!m_needSendStatus)
    {
        return;
    }

    if (!m_newBlocks && utcTime() <= m_maintainBlocksTimeout)  //往下走的条件 1 新区块上链  2 无新区块上链但超时5s
    {
        return;
    }

    m_newBlocks = false;
    m_maintainBlocksTimeout = utcTime() + c_maintainBlocksTimeout;


    int64_t number = m_blockChain->number();
    h256 const& currentHash = m_blockChain->numberHash(number);  //？？？   引用

    // Just broadcast status
    m_syncStatus->foreachPeer([&](shared_ptr<SyncPeerStatus> _p) {
        SyncStatusPacket packet;
        packet.encode(number, m_genesisHash, currentHash);

        m_service->asyncSendMessageByNodeID(
            _p->nodeId, packet.toMessage(m_protocolId), CallbackFuncWithSession(), Options());

        SYNC_LOG(DEBUG) << LOG_BADGE("Status")
                        << LOG_DESC("Send current status when maintainBlocks")
                        << LOG_KV("number", int(number))
                        << LOG_KV("genesisHash", m_genesisHash.abridged())
                        << LOG_KV("currentHash", currentHash.abridged())
                        << LOG_KV("peer", _p->nodeId.abridged());

        return true;
    });
}

void SyncMaster::maintainPeersStatus()
{
    // need download? ->set syncing and knownHighestNumber
    int64_t currentNumber = m_blockChain->number();
    int64_t maxPeerNumber = 0;
    h256 latestHash;
    m_syncStatus->foreachPeer([&](shared_ptr<SyncPeerStatus> _p) {//找到我知道的 最高区块
        if (_p->number > maxPeerNumber)
        {
            latestHash = _p->latestHash;
            maxPeerNumber = _p->number;
        }
        return true;
    });

    // update my known   //更新 变量 （已知的 最高区块）
    if (maxPeerNumber > currentNumber)
    {
        WriteGuard l(m_syncStatus->x_known);
        m_syncStatus->knownHighestNumber = maxPeerNumber;
        m_syncStatus->knownLatestHash = latestHash;
    }
    else
    {
        // No need to send sync request
        WriteGuard l(m_syncStatus->x_known);
        m_syncStatus->knownHighestNumber = currentNumber;
        m_syncStatus->knownLatestHash = m_blockChain->numberHash(currentNumber);
        return;
    }

    // Not to start download when mining or no need      peer中的最高区块号<= 本节点正在打包的区块号  或者 和本节点最高区块号相同时，不需要去同步
    {
        ReadGuard l(x_currentSealingNumber);
        if (maxPeerNumber <= m_currentSealingNumber || maxPeerNumber == currentNumber) //为什么用m_currentSealingNumber？？？正在共识执行完的区块也不需要去同步
        {
            // mining : maxPeerNumber - currentNumber == 1
            // no need: maxPeerNumber - currentNumber <= 0
            SYNC_LOG(TRACE) << LOG_BADGE("Download") << LOG_DESC("No need to download")
                            << LOG_KV("currentNumber", currentNumber)
                            << LOG_KV("currentSealingNumber", m_currentSealingNumber)
                            << LOG_KV("maxPeerNumber", maxPeerNumber);
            return;  // no need to sync
        }
    }

    // Skip downloading if last if not timeout                              每发送一个区块下载请求后 需要等一会儿在发送同步请求？
                                                                                                                    //每个区块200ms时间
    uint64_t currentTime = utcTime();
    if (((int64_t)currentTime - (int64_t)m_lastDownloadingRequestTime) <
        (int64_t)c_eachBlockDownloadingRequestTimeout * (m_maxRequestNumber - currentNumber))
    {
        SYNC_LOG(DEBUG) << LOG_BADGE("Download") << LOG_DESC("Waiting for peers' blocks")
                        << LOG_KV("currentNumber", currentNumber)
                        << LOG_KV("maxRequestNumber", m_maxRequestNumber)
                        << LOG_KV("maxPeerNumber", maxPeerNumber);
        return;  // no need to sync
    }
    m_lastDownloadingRequestTime = currentTime;

    // Start download
    noteDownloadingBegin();

    // Choose to use min number in blockqueue or max peer number  1   本节点区块号 >= peers中已知的最高区块号 时候，不需要去同步
    int64_t maxRequestNumber = maxPeerNumber;
    BlockPtr topBlock = m_syncStatus->bq().top();
    if (nullptr != topBlock)
    {
        int64_t minNumberInQueue = topBlock->header().number();
        maxRequestNumber = min(maxPeerNumber, minNumberInQueue - 1);//2.本节点区块号 >= 下载队列中最小区块号时，说明正在同步中，不需要去同步
    }
    if (currentNumber >= maxRequestNumber)
    {
        SYNC_LOG(TRACE) << LOG_BADGE("Download")
                        << LOG_DESC("No need to sync when blocks are already in queue")
                        << LOG_KV("currentNumber", currentNumber)
                        << LOG_KV("maxRequestNumber", maxRequestNumber);
        return;  // no need to send request block packet
    }

    // Sharding by c_maxRequestBlocks to request blocks   向每个peer请求最大区块数是32，求出需要发送请求消息的peer的个数
    size_t shardNumber =//请求32个区块内，对一个节点请求，多余32个区块，像2个节点请求
        (maxRequestNumber - currentNumber + c_maxRequestBlocks - 1) / c_maxRequestBlocks;
    size_t shard = 0;//表示从第几个节点取区块

    m_maxRequestNumber = 0;  // each request turn has new m_maxRequestNumber
    while (shard < shardNumber && shard < c_maxRequestShards)
    {
        bool thisTurnFound = false;  //标记用的，当前节点是不是 有最高区块 的节点
        m_syncStatus->foreachPeerRandom([&](std::shared_ptr<SyncPeerStatus> _p) {
            if (m_syncStatus->knownHighestNumber <= 0 ||
                _p->number != m_syncStatus->knownHighestNumber)
            {
                // Only send request to nodes which are not syncing(has max number)          只对有最高区块的节点发送 区块请求
                return true;
            }

            // shard: [from, to]
            int64_t from = currentNumber + 1 + shard * c_maxRequestBlocks;
            int64_t to = min(from + c_maxRequestBlocks - 1, maxRequestNumber);
            if (_p->number < to)
                return true;  // exit, to next peer  没啥用

            // found a peer
            thisTurnFound = true;
            SyncReqBlockPacket packet;
            unsigned size = to - from + 1;
            packet.encode(from, size);
            m_service->asyncSendMessageByNodeID(//给节点发请求区块消息
                _p->nodeId, packet.toMessage(m_protocolId), CallbackFuncWithSession(), Options());

            // update max request number   这条很奇怪，没啥用
            m_maxRequestNumber = max(m_maxRequestNumber, to);

            SYNC_LOG(INFO) << LOG_BADGE("Download") << LOG_BADGE("Request")
                           << LOG_DESC("Request blocks") << LOG_KV("frm", from) << LOG_KV("to", to)
                           << LOG_KV("peer", _p->nodeId.abridged());

            ++shard;  // shard move

            return shard < shardNumber && shard < c_maxRequestShards;
        });

        if (!thisTurnFound)//链接的节点数量 小于 收集的同步状态数量，并且链接的节点高度都小于已知的最高区块
        {
            int64_t from = currentNumber + shard * c_maxRequestBlocks;
            int64_t to = min(from + c_maxRequestBlocks - 1, maxRequestNumber);

            SYNC_LOG(WARNING) << LOG_BADGE("Download") << LOG_BADGE("Request")
                              << LOG_DESC("Couldn't find any peers to request blocks")
                              << LOG_KV("from", from) << LOG_KV("to", to);
            break;
        }
    }
}

bool SyncMaster::maintainDownloadingQueue()
{
    int64_t currentNumber = m_blockChain->number();
    DownloadingBlockQueue& bq = m_syncStatus->bq();
    if (currentNumber >= m_syncStatus->knownHighestNumber)
    {
        bq.clear();
        return true;
    }

    // pop block in sequence and ignore block which number is lower than currentNumber +1
    BlockPtr topBlock = bq.top();
    while (topBlock != nullptr && topBlock->header().number() <= (m_blockChain->number() + 1))
    {
        try
        {
            if (isNextBlock(topBlock))
            {
                auto record_time = utcTime();
                auto parentBlock =
                    m_blockChain->getBlockByNumber(topBlock->blockHeader().number() - 1);
                BlockInfo parentBlockInfo{parentBlock->header().hash(),
                    parentBlock->header().number(), parentBlock->header().stateRoot()};
                auto getBlockByNumber_time_cost = utcTime() - record_time;
                record_time = utcTime();
                SYNC_LOG(INFO) << LOG_BADGE("Download") << LOG_BADGE("BlockSync")
                               << LOG_DESC("Download block execute")
                               << LOG_KV("number", topBlock->header().number())
                               << LOG_KV("txs", topBlock->transactions().size())
                               << LOG_KV("hash", topBlock->headerHash().abridged());
                ExecutiveContext::Ptr exeCtx =
                    m_blockVerifier->executeBlock(*topBlock, parentBlockInfo);

                if (exeCtx == nullptr)
                {
                    bq.pop();
                    topBlock = bq.top();
                    continue;
                }

                auto executeBlock_time_cost = utcTime() - record_time;
                record_time = utcTime();

                CommitResult ret = m_blockChain->commitBlock(*topBlock, exeCtx);
                auto commitBlock_time_cost = utcTime() - record_time;
                record_time = utcTime();
                if (ret == CommitResult::OK)
                {
                    m_txPool->dropBlockTrans(*topBlock);
                    auto dropBlockTrans_time_cost = utcTime() - record_time;
                    SYNC_LOG(INFO) << LOG_BADGE("Download") << LOG_BADGE("BlockSync")
                                   << LOG_DESC("Download block commit succ")
                                   << LOG_KV("number", topBlock->header().number())
                                   << LOG_KV("txs", topBlock->transactions().size())
                                   << LOG_KV("hash", topBlock->headerHash().abridged())
                                   << LOG_KV("getBlockByNumberTimeCost", getBlockByNumber_time_cost)
                                   << LOG_KV("executeBlockTimeCost", executeBlock_time_cost)
                                   << LOG_KV("commitBlockTimeCost", commitBlock_time_cost)
                                   << LOG_KV("dropBlockTransTimeCost", dropBlockTrans_time_cost);
                }
                else
                {
                    SYNC_LOG(ERROR) << LOG_BADGE("Download") << LOG_BADGE("BlockSync")
                                    << LOG_DESC("Block commit failed")
                                    << LOG_KV("number", topBlock->header().number())
                                    << LOG_KV("txs", topBlock->transactions().size())
                                    << LOG_KV("hash", topBlock->headerHash().abridged());
                }
            }
            else
            {
                SYNC_LOG(DEBUG) << LOG_BADGE("Download") << LOG_BADGE("BlockSync")
                                << LOG_DESC("Block of queue top is not the next block")
                                << LOG_KV("number", topBlock->header().number())
                                << LOG_KV("txs", topBlock->transactions().size())
                                << LOG_KV("hash", topBlock->headerHash().abridged());
            }
        }
        catch (exception& e)
        {
            SYNC_LOG(ERROR) << LOG_BADGE("Download") << LOG_BADGE("BlockSync")
                            << LOG_DESC("Block of queue top is not a valid block")
                            << LOG_KV("number", topBlock->header().number())
                            << LOG_KV("txs", topBlock->transactions().size())
                            << LOG_KV("hash", topBlock->headerHash().abridged());
        }

        bq.pop();
        topBlock = bq.top();
    }


    currentNumber = m_blockChain->number();
    // has this request turn finished ?
    if (currentNumber >= m_maxRequestNumber)
        m_lastDownloadingRequestTime = 0;  // reset it to trigger request immediately

    // has download finished ?
    if (currentNumber >= m_syncStatus->knownHighestNumber)
    {
        h256 const& latestHash =
            m_blockChain->getBlockByNumber(m_syncStatus->knownHighestNumber)->headerHash();
        SYNC_LOG(INFO) << LOG_BADGE("Download") << LOG_BADGE("BlockSync")
                       << LOG_DESC("Download finish") << LOG_KV("latestHash", latestHash.abridged())
                       << LOG_KV("expectedHash", m_syncStatus->knownLatestHash.abridged());

        if (m_syncStatus->knownLatestHash != latestHash)
            SYNC_LOG(ERROR)
                << LOG_BADGE("Download")
                << LOG_DESC(
                       "State error: This node's version is not compatable with others! All data "
                       "should be cleared of this node before restart");

        return true;
    }
    return false;
}

void SyncMaster::maintainPeersConnection()
{
    // Get active peers
    auto sessions = m_service->sessionInfosByProtocolID(m_protocolId);
    set<NodeID> activePeers;
    for (auto const& session : sessions)
    {
        activePeers.insert(session.nodeID());
    }

    // Get sealers and observer
    NodeIDs sealers = m_blockChain->sealerList();
    NodeIDs sealerOrObserver = sealers + m_blockChain->observerList();

    // member set is [(sealer || observer) && activePeer && not myself]   //获取可链接的节点
    set<NodeID> memberSet;
    bool hasMyself = false;//本节点时候在共识列表中
    for (auto const& member : sealerOrObserver)
    {
        /// find active peers
        if (activePeers.find(member) != activePeers.end() && member != m_nodeId)
        {
            memberSet.insert(member);
        }
        hasMyself |= (member == m_nodeId);
    }

    // Delete uncorrelated peers
    int64_t currentNumber = m_blockChain->number();
    NodeIDs peersToDelete;
    m_syncStatus->foreachPeer([&](std::shared_ptr<SyncPeerStatus> _p) {
        NodeID id = _p->nodeId;
        if (memberSet.find(id) == memberSet.end() && currentNumber >= _p->number)//删除不活跃的同步节点 高度比自己高的不活跃也不删除
        {
            // Only delete outsider whose number is smaller than myself
            peersToDelete.emplace_back(id);
        }
        return true;
    });

    for (NodeID const& id : peersToDelete)
    {
        m_syncStatus->deletePeer(id);
    }


    // Add new peers
    h256 const& currentHash = m_blockChain->numberHash(currentNumber); //根据链接  增加新的活跃节点到 同步节点集中
    for (auto const& member : memberSet)
    {
        if (member != m_nodeId && !m_syncStatus->hasPeer(member))
        {
            // create a peer
            SyncPeerInfo newPeer{member, 0, m_genesisHash, m_genesisHash};
            m_syncStatus->newSyncPeerStatus(newPeer);

            if (m_needSendStatus)
            {
                // send my status to her                            //发送本节点状态 给 新加入的节点
                SyncStatusPacket packet;
                packet.encode(currentNumber, m_genesisHash, currentHash);
                m_service->asyncSendMessageByNodeID(
                    member, packet.toMessage(m_protocolId), CallbackFuncWithSession(), Options());
                SYNC_LOG(DEBUG) << LOG_BADGE("Status")
                                << LOG_DESC("Send current status to new peer")
                                << LOG_KV("number", int(currentNumber))
                                << LOG_KV("genesisHash", m_genesisHash.abridged())
                                << LOG_KV("currentHash", currentHash.abridged())
                                << LOG_KV("peer", member.abridged());
            }
        }
    }

    // Update sync sealer status  //更新 同步节点集中的节点 sealer 标识
    set<NodeID> sealerSet;
    for (auto sealer : sealers)
        sealerSet.insert(sealer);

    m_syncStatus->foreachPeer([&](shared_ptr<SyncPeerStatus> _p) {
        _p->isSealer = (sealerSet.find(_p->nodeId) != sealerSet.end());
        return true;
    });

    // If myself is not in group, no need to maintain transactions(send transactions to peers)
    m_needMaintainTransactions = hasMyself;

    // If myself is not in group, no need to maintain blocks(send sync status to peers)
    m_needSendStatus = hasMyself;
}

void SyncMaster::maintainDownloadingQueueBuffer()
{
    if (m_syncStatus->state == SyncState::Downloading)  //进入下载流程  则把缓存导入 下载队列
    {
        m_syncStatus->bq().clearFullQueueIfNotHas(m_blockChain->number() + 1);
        m_syncStatus->bq().flushBufferToQueue();//把下载的区块缓存 倒入 下载队列，并清空缓存
    }
    else
        m_syncStatus->bq().clear(); //若处于idle，清空下载队列 和 下载缓存
}

void SyncMaster::maintainBlockRequest()
{
    uint64_t timeout = utcTime() + c_respondDownloadRequestTimeout;
    m_syncStatus->foreachPeerRandom([&](std::shared_ptr<SyncPeerStatus> _p) {
        DownloadRequestQueue& reqQueue = _p->reqQueue;
        if (reqQueue.empty())
            return true;  // no need to respeond

        // Just select one peer per maintain
        reqQueue.disablePush();  // drop push at this time
        DownloadBlocksContainer blockContainer(m_service, m_protocolId, _p->nodeId);

        while (!reqQueue.empty() && utcTime() <= timeout)//200ms的时间    ？？？sync
        {
            DownloadRequest req = reqQueue.topAndPop();//每看懂？？？sync
            int64_t number = req.fromNumber;
            int64_t numberLimit = req.fromNumber + req.size;//[number, numberLimit)

            // Send block at sequence
            for (; number < numberLimit && utcTime() <= timeout; number++)
            {
                auto start_get_block_time = utcTime();
                shared_ptr<bytes> blockRLP = m_blockChain->getBlockRLPByNumber(number);
                if (!blockRLP)
                {
                    SYNC_LOG(WARNING)
                        << LOG_BADGE("Download") << LOG_BADGE("Request")
                        << LOG_DESC("Get block for node failed")
                        << LOG_KV("reason", "block is null") << LOG_KV("number", number)
                        << LOG_KV("nodeId", _p->nodeId.abridged());
                    break;
                }

                SYNC_LOG(DEBUG) << LOG_BADGE("Download") << LOG_BADGE("Request")
                                << LOG_BADGE("BlockSync") << LOG_DESC("Batch blocks for sending")
                                << LOG_KV("number", number) << LOG_KV("peer", _p->nodeId.abridged())
                                << LOG_KV("timeCost", utcTime() - start_get_block_time);
                blockContainer.batchAndSend(blockRLP);
            }

            if (number < numberLimit)  // This respond not reach the end due to timeout
            {
                // write back the rest request range
                reqQueue.enablePush();
                SYNC_LOG(DEBUG) << LOG_BADGE("Download") << LOG_BADGE("Request")
                                << LOG_DESC("Push unsent requests back to reqQueue")
                                << LOG_KV("from", number) << LOG_KV("to", numberLimit - 1)
                                << LOG_KV("peer", _p->nodeId.abridged());
                reqQueue.push(number, numberLimit - number);
                return false;
            }
        }

        reqQueue.enablePush();
        return false;
    });
}

bool SyncMaster::isNextBlock(BlockPtr _block)
{
    if (_block == nullptr)
        return false;

    int64_t currentNumber = m_blockChain->number();
    if (currentNumber + 1 != _block->header().number())
    {
        SYNC_LOG(WARNING) << LOG_BADGE("Download") << LOG_BADGE("BlockSync")
                          << LOG_DESC("Ignore illegal block") << LOG_KV("reason", "number illegal")
                          << LOG_KV("thisNumber", _block->header().number())
                          << LOG_KV("currentNumber", currentNumber);
        return false;
    }

    if (m_blockChain->numberHash(currentNumber) != _block->header().parentHash())
    {
        SYNC_LOG(WARNING) << LOG_BADGE("Download") << LOG_BADGE("BlockSync")
                          << LOG_DESC("Ignore illegal block")
                          << LOG_KV("reason", "parent hash illegal")
                          << LOG_KV("thisNumber", _block->header().number())
                          << LOG_KV("currentNumber", currentNumber)
                          << LOG_KV("thisParentHash", _block->header().parentHash().abridged())
                          << LOG_KV(
                                 "currentHash", m_blockChain->numberHash(currentNumber).abridged());
        return false;
    }

    // check block sealerlist sig
    if (fp_isConsensusOk && !(fp_isConsensusOk)(*_block))
    {
        SYNC_LOG(WARNING) << LOG_BADGE("Download") << LOG_BADGE("BlockSync")
                          << LOG_DESC("Ignore illegal block")
                          << LOG_KV("reason", "consensus check failed")
                          << LOG_KV("thisNumber", _block->header().number())
                          << LOG_KV("currentNumber", currentNumber)
                          << LOG_KV("thisParentHash", _block->header().parentHash().abridged())
                          << LOG_KV(
                                 "currentHash", m_blockChain->numberHash(currentNumber).abridged());
        return false;
    }

    return true;
}
