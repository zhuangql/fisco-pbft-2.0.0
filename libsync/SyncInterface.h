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
 * @brief : external interfaces of BlockSync module called by RPC and p2p
 * @author: yujiechen, jimmyshi
 * @date: 2018-09-21
 */
#pragma once
#include "Common.h"
#include <libdevcore/FixedHash.h>
#include <libdevcore/Worker.h>
namespace dev
{
namespace sync
{
struct SyncStatus;
class SyncInterface
{
public:
    SyncInterface(){};
    virtual ~SyncInterface(){};
    /// start blockSync
    virtual void start() = 0;
    /// stop blockSync
    virtual void stop() = 0;

    /// get status of block sync
    /// @returns Synchonization status
    virtual SyncStatus status() const = 0;

    /// get all sync info as string for rpc
    virtual std::string const syncInfo() const = 0;

    /// note sync module the sealing number
    virtual void noteSealingBlockNumber(int64_t _number) = 0;

    virtual bool isSyncing() const = 0;

    // is my number is far smaller than max block number of this block chain
    virtual bool isFarSyncing() const = 0;

    /// protocol id used when register handler to p2p module
    virtual PROTOCOL_ID const& protocolId() const = 0;
    virtual void setProtocolId(PROTOCOL_ID const _protocolId) = 0;

    // verify handler to check downloading block
    virtual void registerConsensusVerifyHandler(
        std::function<bool(dev::eth::Block const&)> _handler) = 0;
};

}  // namespace sync
}  // namespace dev
