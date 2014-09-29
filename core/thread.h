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
    //�����߳�
    bool Start(CTreadPool* pInputThreadPool = NULL);
    //�����߳�
    void Active();
    //һ��Ҫ��ʾ�����˳��̣߳������������˳��߳�
    void ExitThread();
    virtual void Run();
private:
    static void* startHook(void* pInput);
    CThread(CThread&);
    CThread& operator =(CThread &);
protected:
    CTreadPool* m_pCTreadPool;
private:
    //�߳�����ID
    pid_t m_pid;
    pthread_t m_thread;
    sem_t m_sem;//�ź���
    bool m_bStart;
    bool m_bNoticeThreadExitFlag;
    bool m_bThreadExitFlag;
};

#endif
