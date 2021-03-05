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
 * @brief : implementation of Consensus
 * @file: Consensus.cpp
 * @author: yujiechen
 * @date: 2018-09-27
 *
 * @ author: yujiechen
 * @ date: 2018-10-26
 * @ file : Sealer.cpp
 * @ modification: rename Consensus.cpp to Sealer.cpp
 */
#include "Sealer.h"
#include <libethcore/LogEntry.h>
#include <libsync/SyncStatus.h>
using namespace std;
using namespace dev::sync;
using namespace dev::blockverifier;
using namespace dev::eth;
using namespace dev::p2p;
using namespace dev::consensus;

/// start the Sealer module
void Sealer::start()
{
    if (m_startConsensus)
    {
        SEAL_LOG(WARNING) << "[Sealer module has already been started]";
        return;
    }
    SEAL_LOG(INFO) << "[Start sealer module]";
    //sealling区块重置空
    resetSealingBlock();
    //更新engineEnv
    m_consensusEngine->reportBlock(*(m_blockChain->getBlockByNumber(m_blockChain->number())));
    //sealer start时从engine取更新后配置的最大交易数量。
    m_maxBlockCanSeal = m_consensusEngine->maxBlockTransactions();
    //一个区块已经被commit
    m_syncBlock = false;
    /// start  a thread to execute doWork()&&workLoop()
    startWorking();
    m_startConsensus = true;
}
//PBFTsealer中还有此方法
bool Sealer::shouldSeal()
{
    bool sealed = false;
    {
        ReadGuard l(x_sealing);
        sealed = m_sealing.block->isSealed();
    }
    //1、区块是空的 ，即没有sealer，则应该seal  （最重要，只关注这个就好）
    //2、sealler封装线程开始 
    //3、本节点是sealer类型，从engine获取  4、同步区块模块没有在工作 ？ zhuangql
    return (!sealed && m_startConsensus &&
            m_consensusEngine->accountType() == NodeAccountType::SealerAccount &&
            !isBlockSyncing());
}

void Sealer::reportNewBlock()
{
    bool t = true;
    //创世块m_sy-ncBlock为false，不进入if语句执行。
    //区块被commit到区块链上后，回调函数使得m_syncBloc-k为true，
    //执行此函数向engine报告最新高度的区块，更新engine环境
    if (m_syncBlock.compare_exchange_strong(t, false))
    {
        shared_ptr<dev::eth::Block> p_block =
            m_blockChain->getBlockByNumber(m_blockChain->number());
        if (!p_block)
        {
            LOG(ERROR) << "[reportNewBlock] empty block";
            return;
        }
        m_consensusEngine->reportBlock(*p_block);
        WriteGuard l(x_sealing);
        {
            //需要看一下 zhuangql
            if (shouldResetSealing())
            {
                SEAL_LOG(DEBUG) << "[reportNewBlock] Reset sealing: [number]:  "
                                << m_blockChain->number()
                                << ", sealing number:" << m_sealing.block->blockHeader().number();
                resetSealingBlock();
            }
        }
    }
}

bool Sealer::shouldWait(bool const& wait) const
{
    return !m_syncBlock && wait;
}

void Sealer::doWork(bool wait)
{
    //上一个区块被commit后，向engine报告新区块的状态环境                                        //创世时不进来
    reportNewBlock();
    //以下分析不足
     //区块是空的 ，即没有sealer，则应该seal    && engine中区块已经被执行 即m_notifyNextLeader Seal = true
     //两种情况：1、正常轮到本节点seal  2、下一个节点提前seal也能进入开始seal，此时sealling中有filter交易
    if (shouldSeal() && m_startConsensus.load())
    {
        WriteGuard l(x_sealing);
        {
            /// get current transaction num
            //当前轮leader返回0    
            //下一轮leader提前seal返回上一个leader区块的交易数（优化）                         //创世时返回0  应该一直返回0？
            uint64_t tx_num = m_sealing.block->getTransactionSize();

            /// add this to in case of unlimited-loop
            //交易池队列的size时候为0，标记是否同步交易池，即load交易
            if (m_txPool->status().current == 0)
            {
                m_syncTxPool = false;
            }
            else
            {
                m_syncTxPool = true;
            }
            //区块最大交易数量，start时从engine只取一次（关闭动态调整区块大小时）
            auto maxTxsPerBlock = maxBlockCanSeal();
            /// load transaction from transaction queue 可以打包交易的条件：
            //区块中交易不到最大交易数 &&交易池不为空 是基本条件 另外：
            //1、时间没到达 1s情况下  区块中交易为0   2、时间没到达 1s，区块中有交易 情况下，没达到0.5s
            if (maxTxsPerBlock > tx_num && m_syncTxPool == true && !reachBlockIntervalTime())
                loadTransactions(maxTxsPerBlock - tx_num);
            /// check enough or reach block interval
            if (!checkTxsEnough(maxTxsPerBlock))//需要看一下里面  zhuangql
            {
                ///< 10 milliseconds to next loop
                boost::unique_lock<boost::mutex> l(x_signalled);
                m_signalled.wait_for(l, boost::chrono::milliseconds(1));// 为什么等待 zhuangql
                return;
            }
            if (shouldHandleBlock())
                handleBlock();
        }
    }
    if (shouldWait(wait))
    {
        boost::unique_lock<boost::mutex> l(x_blocksignalled);
        m_blockSignalled.wait_for(l, boost::chrono::milliseconds(10));//因为什么等待 zhuangql
    }
}

/**
 * @brief: load transactions from the transaction pool
 * @param transToFetch: max transactions to fetch
 */
void Sealer::loadTransactions(uint64_t const& transToFetch)
{
    /// fetch transactions and update m_transactionSet
    m_sealing.block->appendTransactions(
        m_txPool->topTransactions(transToFetch, m_sealing.m_transactionSet, true));
}

/// check whether the blocksync module is syncing
bool Sealer::isBlockSyncing()
{
    SyncStatus state = m_blockSync->status();
    return (state.state != SyncState::Idle);
}

/**
 * @brief : reset specified sealing block by generating an empty block
 * 
 * @param sealing :  the block should be resetted
 * 
 * 
 * @param filter : the tx hashes of transactions that should't be packeted into sealing block when
 * loadTransactions(used to set m_transactionSet)
 * @param resetNextLeader : reset realing for the next leader or not ? default is false.
 *                          true: reset sealing for the next leader; the block number of the sealing
 * header should be reset to the current block number add 2 
 * false: reset sealing for the current
 * leader; the sealing header should be populated from the current block
 */   //就是置空sealling区块
 //1、置空当前共识轮区块
 //2、置空下一个leader的区块
void Sealer::resetSealingBlock(Sealing& sealing, h256Hash const& filter, bool resetNextLeader)
{
    resetBlock(sealing.block, resetNextLeader);
    sealing.m_transactionSet = filter;
    sealing.p_execContext = nullptr;
}

/**
 * @brief : reset specified block according to 'resetNextLeader' option
 *
 * @param block : the block that should be resetted
 * @param resetNextLeader: reset the block for the next leader or not ? default is false.
 *                         true: reset block for the next leader; the block number of the block
 * header should be reset to the current block number add 2 false: reset block for the current
 * leader; the block header should be populated from the current block
 */
void Sealer::resetBlock(std::shared_ptr<dev::eth::Block> block, bool resetNextLeader)
{
    /// reset block for the next leader:
    /// 1. clear the block; 2. set the block number to current block number add 2
    //出现空块或者iewchange时候看这段代码
    if (resetNextLeader)
    {
        SEAL_LOG(DEBUG) << "reset nextleader number to:" << (m_blockChain->number() + 2);
        block->resetCurrentBlock();
        block->header().setNumber(m_blockChain->number() + 2);
    }
    /// reset block for current leader:
    /// 1. clear the block; 2. populate header from the highest block
    else
    {
        auto highestBlock = m_blockChain->getBlockByNumber(m_blockChain->number());
        if (!highestBlock)
        {  // impossible so exit
            SEAL_LOG(FATAL) << LOG_DESC("exit because can't get highest block")
                            << LOG_KV("number", m_blockChain->number());
        }
        block->resetCurrentBlock(highestBlock->blockHeader());
        SEAL_LOG(DEBUG) << "resetCurrentBlock to"
                        << LOG_KV("sealingNum", block->blockHeader().number());
    }
}

/**
 * @brief : set some important fields for specified block header (called by PBFTSealer after load
 * transactions finished)
 *
 * @param header : the block header should be setted
 * the resetted fields including to:
 * 1. block import time;
 * 2. sealer list: reset to current leader list
 * 3. sealer: reset to the idx of the block generator
 */
void Sealer::resetSealingHeader(BlockHeader& header)
{
    /// import block
    resetCurrentTime();
    header.setSealerList(m_consensusEngine->consensusList());
    header.setSealer(m_consensusEngine->nodeIdx());
    header.setLogBloom(LogBloom());
    header.setGasUsed(u256(0));
    header.setExtraData(m_extraData);
}

/// stop the Sealer module
void Sealer::stop()
{
    if (m_startConsensus == false)
    {
        return;
    }
    SEAL_LOG(INFO) << "Stop sealer module...";
    m_startConsensus = false;
    doneWorking();
    if (isWorking())
    {
        stopWorking();
        // will not restart worker, so terminate it
        terminate();
    }
}
