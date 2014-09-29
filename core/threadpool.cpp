/**
*Unit Name: mypool.cpp
*Current Version: 1.0
*Description:
*   线程池.
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
CTreadPool::CTreadPool():m_CurNodeNum(0), m_Mutex(), m_bInitalFlag(false)
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

bool CTreadPool::InitialPool(unsigned int nThreadNum)
{
    Lock lock(m_Mutex);
    for (unsigned int i = 0; (i < nThreadNum) && (i < MAX_THREAD_NUM); i++ )
    {
        CThread* pThread = CreateThread();
        if ( pThread->Start(this) != true)
        {
            delete pThread;
            trace_log(ERR, "Creating thread is failed");
            return false;
        }
        time_t CurTime = 0;
        time(&CurTime);
        m_FreeThreadArray[pThread] = CurTime;

    }
    m_bInitalFlag = true;
    return true;
}
CThread* CTreadPool::CreateThread()
{
    CThread* pThread = new CThread();
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
    void* pThread = m_Data_Pool[m_CurNodeNum -1];
    m_Data_Pool[m_CurNodeNum -1] = NULL;
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
void CTreadPool::ReleaseThread(CThread* pThread)
{
    Lock lock(m_Mutex);
    time_t CurTime = 0;
    //空闲线程无需记录时间
    m_FreeThreadArray[pThread] = CurTime;
    m_BusyThreadArray.erase(pThread);
    trace_log(DBG, "ReleaseThread free num:%lu busy num:%lu ",
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
    for (;iter != ThreadPool.end();++iter)
    {
        CThread* pThread = iter->first;
        delete pThread;
    }
    ThreadPool.clear();
}
