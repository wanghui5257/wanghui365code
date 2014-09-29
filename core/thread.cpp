/**
 *Unit Name: mythread.cpp
 *Current Version: 1.0
 *Description:
 *   MyThread class.
 *Author:
 *   wanghui@xgd.com
 *Copyright:
 *   Copyright(C) 2013.07
 *History:
 *
 *TODO:
**/
#include <unistd.h>
#include "log.h"
#include "Thread.h"

CThread::CThread():m_pCTreadPool(NULL), m_bStart(false),
                  m_bNoticeThreadExitFlag(false), m_bThreadExitFlag(false)
{
}

CThread::~CThread()
{
    if (m_bStart)
    {
        ExitThread();
    }
    sem_destroy(&m_sem);
}

bool CThread::Start(CTreadPool* pInputThreadPool)
{
    //初始化信号量，其初值为0
    int res = sem_init(&m_sem, 0, 0);
    if(res == -1)
    {
        trace_log(ERR, "semaphore intitialization failed");
        return false;
    }

    if (pthread_create(&m_thread, NULL, startHook, this) != 0)
    {
        trace_log(ERR,"starting thread is failed");
        return false;
    }
    m_pCTreadPool = pInputThreadPool;
    m_bStart = true;
    return true;
}

//线程回调钩子运行时，线程已经启动成功
void* CThread::startHook(void* pInput)
{
    CThread *pThreadInstance = (CThread *)pInput;
    //分离线程
    pthread_detach(pThreadInstance->m_thread);
    pThreadInstance->m_pid = gettid();
    trace_log(DBG,"start thread id: %d",pThreadInstance->m_pid);
    while (1)
    {
        //异常信号，不运行
        int iRet = sem_wait(&pThreadInstance->m_sem);
        if ( iRet != 0)
        {
            trace_log(DBG,"thread m_sem is error ret code:%d", iRet);
            continue;
        }

        if (pThreadInstance->m_bNoticeThreadExitFlag)
        {
            trace_log(DBG,"thread id: %d exit", pThreadInstance->m_pid);
            pThreadInstance->m_bThreadExitFlag = true;
            pthread_exit(NULL);
        }
        pThreadInstance->Run();

        if (pThreadInstance->m_pCTreadPool != NULL)
        {
            pThreadInstance->m_pCTreadPool->ReleaseThread(pThreadInstance);
        }
    }

    return NULL;
}

void CThread::Active()
{
    if (!m_bStart)
    {
        trace_log(ERR,"thread id: %d isn't running", m_pid);
        return;
    }
    sem_post(&m_sem);
}
void CThread::Run()
{
    trace_log(DBG,"thread id: %d is running", m_pid);
}
void CThread::ExitThread()
{
    if (!m_bStart)
    {
        return;
    }
    m_bNoticeThreadExitFlag = true;
    sem_post(&m_sem);
    //等待一秒
    unsigned int nCount = 0;
    while(!m_bThreadExitFlag && nCount < 1000)
    {
        usleep(1000);
        nCount++;
    }

    if(!m_bThreadExitFlag)
    {
        pthread_cancel(m_thread);
    }
}
