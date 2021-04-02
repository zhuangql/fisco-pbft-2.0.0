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
 * @brief : The implementation of callback from p2p
 * @author: jimmyshi
 * @date: 2018-10-17
 */

#pragma once
#include "Common.h"
#include "DownloadingTxsQueue.h"
#include "RspBlockReq.h"
#include "SyncMsgPacket.h"
#include "SyncStatus.h"
#include <libblockchain/BlockChainInterface.h>
#include <libdevcore/FixedHash.h>
#include <libdevcore/Worker.h>
#include <libethcore/Exceptions.h>
#include <libnetwork/Common.h>
#include <libnetwork/Session.h>
#include <libp2p/P2PInterface.h>
#include <libp2p/P2PMessage.h>
#include <libtxpool/TxPoolInterface.h>

namespace dev
{
namespace sync
{
class SyncMsgEngine
{
public:
    SyncMsgEngine(std::shared_ptr<dev::p2p::P2PInterface> _service,
        std::shared_ptr<dev::txpool::TxPoolInterface> _txPool,
        std::shared_ptr<dev::blockchain::BlockChainInterface> _blockChain,
        std::shared_ptr<SyncMasterStatus> _syncStatus,
        std::shared_ptr<DownloadingTxsQueue> _txQueue, PROTOCOL_ID const& _protocolId,
        NodeID const& _nodeId, h256 const& _genesisHash)
      : m_service(_service),
        m_txPool(_txPool),
        m_blockChain(_blockChain),
        m_syncStatus(_syncStatus),
        m_txQueue(_txQueue),
        m_protocolId(_protocolId),
        m_groupId(dev::eth::getGroupAndProtocol(_protocolId).first),
        m_nodeId(_nodeId),
        m_genesisHash(_genesisHash)
    {
        m_service->registerHandlerByProtoclID(
            m_protocolId, boost::bind(&SyncMsgEngine::messageHandler, this, _1, _2, _3));
    }

    void messageHandler(dev::p2p::NetworkException _e,
        std::shared_ptr<dev::p2p::P2PSession> _session, dev::p2p::P2PMessage::Ptr _msg);

private:
    bool checkSession(std::shared_ptr<dev::p2p::P2PSession> _session);
    bool checkMessage(dev::p2p::P2PMessage::Ptr _msg);
    bool checkGroupPacket(SyncMsgPacket const& _packet);
    bool interpret(SyncMsgPacket const& _packet);

private:
    void onPeerStatus(SyncMsgPacket const& _packet);
    void onPeerTransactions(SyncMsgPacket const& _packet);
    void onPeerBlocks(SyncMsgPacket const& _packet);
    void onPeerRequestBlocks(SyncMsgPacket const& _packet);

private:
    // Outside data
    std::shared_ptr<dev::p2p::P2PInterface> m_service;
    std::shared_ptr<dev::txpool::TxPoolInterface> m_txPool;
    std::shared_ptr<dev::blockchain::BlockChainInterface> m_blockChain;
    std::shared_ptr<SyncMasterStatus> m_syncStatus;
    std::shared_ptr<DownloadingTxsQueue> m_txQueue;

    // Internal data
    PROTOCOL_ID m_protocolId;
    GROUP_ID m_groupId;
    NodeID m_nodeId;  ///< Nodeid of this node
    h256 m_genesisHash;
};

class DownloadBlocksContainer
{
public:
    DownloadBlocksContainer(
        std::shared_ptr<dev::p2p::P2PInterface> _service, PROTOCOL_ID _protocolId, NodeID _nodeId)
      : m_service(_service), m_protocolId(_protocolId), m_nodeId(_nodeId), m_blockRLPsBatch()
    {
        m_groupId = dev::eth::getGroupAndProtocol(_protocolId).first;
    }
    ~DownloadBlocksContainer() { clearBatchAndSend(); }

    void batchAndSend(BlockPtr _block);
    void batchAndSend(std::shared_ptr<bytes> _blockRLP);

private:
    void clearBatchAndSend();
    void sendBigBlock(bytes const& _blockRLP);

private:
    std::shared_ptr<dev::p2p::P2PInterface> m_service;
    PROTOCOL_ID m_protocolId;
    GROUP_ID m_groupId;
    NodeID m_nodeId;
    std::vector<dev::bytes> m_blockRLPsBatch;
    size_t m_currentBatchSize = 0;
};

}  // namespace sync
}  // namespace dev
