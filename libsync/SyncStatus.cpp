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
 * @brief : Sync peer
 * @author: jimmyshi
 * @date: 2018-10-16
 */

#include "SyncStatus.h"


using namespace std;
using namespace dev;
using namespace dev::eth;
using namespace dev::sync;
using namespace dev::p2p;
using namespace dev::blockchain;
using namespace dev::txpool;

bool SyncMasterStatus::hasPeer(NodeID const& _id)
{
    ReadGuard l(x_peerStatus);
    auto peer = m_peersStatus.find(_id);
    return peer != m_peersStatus.end();
}

bool SyncMasterStatus::newSyncPeerStatus(SyncPeerInfo const& _info)
{
    if (hasPeer(_info.nodeId))
    {
        SYNC_LOG(WARNING) << LOG_BADGE("Status")
                          << LOG_DESC("Peer status is exist, no need to create");
        return false;
    }

    try
    {
        shared_ptr<SyncPeerStatus> peer = make_shared<SyncPeerStatus>(_info, this->m_protocolId);

        WriteGuard l(x_peerStatus);
        m_peersStatus.insert(pair<NodeID, shared_ptr<SyncPeerStatus>>(peer->nodeId, peer));
    }
    catch (Exception const& e)
    {
        SYNC_LOG(ERROR) << LOG_BADGE("Status") << LOG_DESC("Create SyncPeer failed!");
        BOOST_THROW_EXCEPTION(InvalidSyncPeerCreation() << errinfo_comment(e.what()));
    }
    return true;
}

void SyncMasterStatus::deletePeer(NodeID const& _id)
{
    WriteGuard l(x_peerStatus);
    auto peer = m_peersStatus.find(_id);
    if (peer != m_peersStatus.end())
        m_peersStatus.erase(peer);
}

NodeIDs SyncMasterStatus::peers()
{
    NodeIDs nodeIds;
    ReadGuard l(x_peerStatus);
    for (auto& peer : m_peersStatus)
        nodeIds.emplace_back(peer.first);
    return nodeIds;
}

std::shared_ptr<SyncPeerStatus> SyncMasterStatus::peerStatus(NodeID const& _id)
{
    ReadGuard l(x_peerStatus);
    auto peer = m_peersStatus.find(_id);
    if (peer == m_peersStatus.end())
    {
        SYNC_LOG(WARNING) << LOG_BADGE("Status") << LOG_DESC("Peer data not found")
                          << LOG_KV("nodeId", _id.abridged());
        return nullptr;
    }
    return peer->second;
}

void SyncMasterStatus::foreachPeer(
    std::function<bool(std::shared_ptr<SyncPeerStatus>)> const& _f) const
{
    ReadGuard l(x_peerStatus);
    for (auto& peer : m_peersStatus)
    {
        if (!_f(peer.second))
            break;
    }
}

void SyncMasterStatus::foreachPeerRandom(
    std::function<bool(std::shared_ptr<SyncPeerStatus>)> const& _f) const
{
    ReadGuard l(x_peerStatus);
    if (m_peersStatus.empty())
        return;

    // Get nodeid list
    NodeIDs nodeIds;
    for (auto& peer : m_peersStatus)
        nodeIds.emplace_back(peer.first);

    // Random nodeid list
    for (size_t i = nodeIds.size() - 1; i > 0; --i)
    {
        size_t select = rand() % (i + 1);
        swap(nodeIds[i], nodeIds[select]);
    }

    // access _f() according to the random list   nodelds随机排序的节点id的列表
    for (NodeID const& nodeId : nodeIds)
    {
        auto const& peer = m_peersStatus.find(nodeId);
        if (peer == m_peersStatus.end())
            continue;
        if (!_f(peer->second))
            break;
    }
}

// TODO: return reference
NodeIDs SyncMasterStatus::randomSelection(
    unsigned _percent, std::function<bool(std::shared_ptr<SyncPeerStatus>)> const& _allow)
{
    NodeIDs chosen;
    NodeIDs allowed;

    size_t peerCount = 0;
    foreachPeer([&](std::shared_ptr<SyncPeerStatus> _p) {
        if (_allow(_p))
        {
            allowed.push_back(_p->nodeId);
        }
        ++peerCount;
        return true;
    });

    // TODO optimize it by swap (no need two vectors)
    size_t chosenSize = (peerCount * _percent + 99) / 100;
    chosen.reserve(chosenSize);
    for (unsigned i = chosenSize; i && allowed.size(); i--)
    {
        unsigned n = rand() % allowed.size();
        chosen.push_back(std::move(allowed[n]));
        allowed.erase(allowed.begin() + n);
    }
    return chosen;
}
// TODO: return reference
NodeIDs SyncMasterStatus::randomSelectionSize(
    size_t _maxChosenSize, std::function<bool(std::shared_ptr<SyncPeerStatus>)> const& _allow)
{
    NodeIDs chosen;
    NodeIDs allowed;

    size_t peerCount = 0;
    foreachPeer([&](std::shared_ptr<SyncPeerStatus> _p) {
        if (_allow(_p))
        {
            allowed.push_back(_p->nodeId);
        }
        ++peerCount;
        return true;
    });

    chosen.reserve(_maxChosenSize);
    for (unsigned i = _maxChosenSize; i && allowed.size(); i--)
    {
        unsigned n = rand() % allowed.size();
        chosen.push_back(std::move(allowed[n]));
        allowed.erase(allowed.begin() + n);
    }
    return chosen;
}