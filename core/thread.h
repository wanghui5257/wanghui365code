#ifndef __THREAD_H__
#define __THREAD_H__
#include <cstdio>
#include <pthread.h>
#include <semaphore.h>
#include "threadpool.h"
class CThread
{
public:
    CThread();
    virtual ~CThread();
    //启动线程
    bool Start(CTreadPool* pInputThreadPool = NULL);
    //激活线程
    void Active();
    virtual void Run();
private:
    static void* startHook(void* pInput);
    CThread(CThread&);
    CThread& operator =(CThread &);
    void ExitThread();
protected:
    CTreadPool* m_pCTreadPool;
private:
    //线程自身ID
    pid_t m_pid;
    pthread_t m_thread;
    sem_t m_sem;//信号量
    bool m_bStart;
    bool m_bNoticeThreadExitFlag;
    bool m_bThreadExitFlag;
};

#endif
