/**
*Unit Name: mypool.cpp
*Current Version: 1.0
*Description:
*   �̳߳�.
*Author:
*   24291878@qq.com
*Copyright:
*   Copyright(C) 2014.02
*History:
*TODO:
**/
#include <assert.h>
#include "log.h"
#include "thread.h"
#include "threadpool.h"
CTreadPool::CTreadPool(): m_CurNodeNum(0), m_Mutex(), m_bInitalFlag(false)
{
    m_FreeThreadArray.clear();
    m_BusyThreadArray.clear();

    for (unsigned int i = 0; i < MAX_DATA_NODE_NUM; i++ )
    {
        m_Data_Pool[i] = NULL;
    }
}
CTreadPool::~CTreadPool()
{
    ReleaseAllThread(m_FreeThreadArray);
    ReleaseAllThread(m_BusyThreadArray);
    ReleaseDataInPool();
}

bool CTreadPool::InitialPool(unsigned int nThreadNum, unsigned int nOverTime)
{
    Lock lock(m_Mutex);
    m_nThreadRunOverTime = nOverTime;
    for (unsigned int i = 0; (i < nThreadNum) && (i < MAX_THREAD_NUM); i++ )
    {
        CThread* pThread = CreateThread();
        if (!pThread)
        {
            return false;
        }
    }
    m_bInitalFlag = true;
    return true;
}
CThread* CTreadPool::CreateThread()
{
    CThread* pThread = new CThread();
    if ( !pThread->Start(this) )
    {
        delete pThread;
        trace_log(ERR, "Creating thread is failed");
        return NULL;
    }
    time_t CurTime = 0;
    time(&CurTime);
    m_FreeThreadArray[pThread] = CurTime;
    return pThread;
}
void CTreadPool::Run()
{

}
void CTreadPool::PushData(void* pDataBuffer)
{
    Lock lock(m_Mutex);
    m_Data_Pool[m_CurNodeNum] = pDataBuffer;
    m_CurNodeNum++;
    ActiveFreeThread();
}
void* CTreadPool::PopData()
{
    Lock lock(m_Mutex);
    if (0 == m_CurNodeNum)
    {
        return NULL;
    }
    void* pThread = m_Data_Pool[m_CurNodeNum - 1];
    m_Data_Pool[m_CurNodeNum - 1] = NULL;
    m_CurNodeNum--;
    return pThread;
}
void CTreadPool::ActiveFreeThread()
{
    if (m_FreeThreadArray.empty())
    {
        trace_log(DBG, "There is no thread");
        return;
    }
    mapThreadPool_t::iterator iter = m_FreeThreadArray.begin();
    CThread* pThread = iter->first;
    m_FreeThreadArray.erase(pThread);
    pThread->Active();
    time_t CurTime = 0;
    time(&CurTime);
    m_BusyThreadArray[pThread] = CurTime;
    return;
}
void CTreadPool::ReleaseFreeThread(CThread* pThread)
{
    Lock lock(m_Mutex);
    time_t CurTime = 0;
    //�����߳������¼ʱ��
    m_FreeThreadArray[pThread] = CurTime;
    m_BusyThreadArray.erase(pThread);
    trace_log(DBG, "ReleaseFreeThread free num:%lu busy num:%lu ",
              m_FreeThreadArray.size(), m_BusyThreadArray.size());
}
void CTreadPool::ReleaseDataInPool()
{
    for (unsigned int i = 0; i < m_CurNodeNum; ++i)
    {
        assert(m_Data_Pool[i] != NULL);
        delete [] (char*)m_Data_Pool[i];
        m_Data_Pool[i] = NULL;
    }
}
void CTreadPool::ReleaseAllThread(mapThreadPool_t& ThreadPool)
{
    mapThreadPool_t::iterator iter = ThreadPool.begin();
    for (; iter != ThreadPool.end(); ++iter)
    {
        CThread* pThread = iter->first;
        delete pThread;
    }
    ThreadPool.clear();
}
const unsigned int CTreadPool::GetTotalThreadNum()
{
    Lock lock(m_Mutex);
    return m_FreeThreadArray.size() + m_BusyThreadArray.size();
}
const unsigned int CTreadPool::GetFreeThreadNum()
{
    Lock lock(m_Mutex);
    return m_FreeThreadArray.size();
}
void CTreadPool::DeleteOverTimeThreadFromBusyArray()
{
    Lock lock(m_Mutex);
    //check all thread�� if the thread is overtime�� release it
    mapThreadPool_t::iterator iter = m_BusyThreadArray.begin();
    mapThreadPool_t::iterator iterEnd = m_BusyThreadArray.end();
    for (; iter != iterEnd; ++iter)
    {
        if (iter->second > m_nThreadRunOverTime)
        {
            CThread* pThread = iter->first;
            delete pThread;
            m_BusyThreadArray.erase(pThread);
        }
    }
}
void CTreadPool::CheckAllThread(CTreadPool* pThreadPool)
{
    //traversing all threads, delete one thread when it is overtime.
    pThreadPool->DeleteOverTimeThreadFromBusyArray();
    /*if total number of free thread is 0,add one thread into pool(only total threads
    less than the max number of thread*/
    if (pThreadPool->GetTotalThreadNum() < MAX_THREAD_NUM)
    {
        (void)pThreadPool->CreateThread();
    }

}
