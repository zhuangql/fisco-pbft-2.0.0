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
 * @file: TimeManager.h
 * @author: yujiechen
 * @date: 2018-09-28
 */
#pragma once
#include <libdevcore/Common.h>
namespace dev
{
namespace consensus
{
struct TimeManager
{
    /// the max block generation time
    //共识节点最长打包时间为1000ms，若超过1000ms新区块中打包到的交易数仍为0，
    //共识模块会进入出空块逻辑，空块并不落盘；
    //从config中仅获取一次
    unsigned m_emptyBlockGenTime = 1000;

    /// last execution finish time, only one will be used at last the finish time of executing tx by leader
    uint64_t m_viewTimeout = 3 * m_emptyBlockGenTime;
    //PBFT共识过程中，区块执行的超时时间，最少为3s, supported_version>=v2.6.0时，配置项生效
    //执行超时的结果是怎样？？？ zhuangql  应该是viewchange的超时时间
    uint64_t m_consensusTimeout = 3 * m_emptyBlockGenTime;
    unsigned m_changeCycle = 0;//什么意思？ 在判断prepare合法中有
    // the recently time collected enough signature packages
    uint64_t m_lastSignTime = 0;
    // the recently time reached consensus最新达成共识的时间
    uint64_t m_lastConsensusTime;
    // the recently time receive the rawPrepare and ready to execute the block
    uint64_t m_lastAddRawPrepareTime = 0;
    // the recently time executed the block
    uint64_t m_lastExecTime = 0;

    /// the minimum block generation time(default is 500ms)
    //考虑到PBFT模块打包太快会导致某些区块中仅打包1到2个很少的交易，浪费存储空间，
    //FISCO BCOS v2.0.0-rc2在群组可变配置group.group_id.ini的[consensus]下引入
    //min_block_generation_time配置项来控制PBFT共识打包的最短时间，
    //即：共识节点打包时间超过min_block_generation_time且打包的交易数大于0才会开始共识流程，处理打包生成的新区块。
    //！！共识节点最长打包时间为1000ms，若超过1000ms新区块中打包到的交易数仍为0，共识模块会进入出空块逻辑，空块并不落盘；
    //min_block_generation_time 不可超过出空块时间1000ms，若设置值超过1000ms，系统默认min_block_generation_time为500ms
    unsigned m_minBlockGenTime = 500;   //v2.0.0-rc2在群组可变配置group.group_id.ini的[consensus]

    /// time point of last signature collection
    //？？？没懂  zhuangql
    std::chrono::steady_clock::time_point m_lastGarbageCollection;
    const unsigned kMaxChangeCycle = 20;
    const unsigned CollectInterval = 60;

    void resetConsensusTimeout(unsigned const& _consensusTimeout)
    {
        m_consensusTimeout = _consensusTimeout;
    }

    inline void initTimerManager(unsigned view_timeout)
    {
        m_lastAddRawPrepareTime = utcSteadyTime();
        m_lastConsensusTime = utcSteadyTime();
        m_lastExecTime = utcSteadyTime();
        m_lastSignTime = 0;
        m_viewTimeout = view_timeout;
        m_changeCycle = 0;
        m_lastGarbageCollection = std::chrono::steady_clock::now();
    }

    inline void changeView()
    {
        m_lastConsensusTime = 0;
        m_lastSignTime = 0;
    }

    inline void updateChangeCycle()
    {
        m_changeCycle = std::min(m_changeCycle + 1, (unsigned)kMaxChangeCycle);
    }

    virtual bool isTimeout(int64_t _now = utcSteadyTime())
    {
        //m_lastConsensusTime的设置有两次：1、新区块上链 2、viewChange发生
        //m_lastSignT ime   刚进入commit 阶段的时间
        //取max的原因：只要一个节点完成prepare阶段，理论上如果没有时间限制，可信节点达成commit共识的成功率很高
        auto maxConsensusTime = std::max(m_lastConsensusTime, m_lastSignTime);
        auto last = maxConsensusTime;
        // collect PBFT related message packets and set the timeout to 3s  收集消息包超时产生viewChange的时间 3s
        auto viewTimeOut = m_viewTimeout;
        // the case that received the rawPrepare, but not collect enough sign requests
        if (m_lastAddRawPrepareTime > maxConsensusTime &&//收到了rawPrepare，但没完成prepare阶段
            (m_lastConsensusTime != 0 || m_lastSignTime != 0))
        {
            // process block execution, set the timeout to consensus_timeout 
            if (m_lastAddRawPrepareTime > m_lastExecTime)
            {   //此时正在执行区块 ？？？  异步执行吗  zhuangql
                viewTimeOut = m_consensusTimeout;
                last = m_lastAddRawPrepareTime;
            }
            // the block has been executed, while not collect enough sign requests
            else
            {   //区块执行完
                last = m_lastExecTime;
            }
        }
        auto interval = (uint64_t)(viewTimeOut * std::pow(1.5, m_changeCycle));
        return (_now - last >= interval);
    }
};
}  // namespace consensus
}  // namespace dev
