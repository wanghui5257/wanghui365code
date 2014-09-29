#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_
#include <map>
#include <time.h>
#include "lock.h"
//最大线程数
#define MAX_THREAD_NUM 32
#define MAX_DATA_NODE_NUM 1024
class CThread;
class CTreadPool
{
public:
    friend class CThread;
    typedef std::map<CThread*, time_t > mapThreadPool_t;
	CTreadPool();
	virtual ~CTreadPool();
	virtual CThread* CreateThread();
	bool InitialPool(unsigned int nThreadNum = MAX_THREAD_NUM);
    virtual void PushData(void* pDataBuffer);
    virtual void* PopData();
    virtual void Run();
    virtual void ReleaseDataInPool();
//void ExitThreadPool();
protected:
    void ReleaseThread(CThread* pThread);
private:
    //forbid copy
    CTreadPool(CTreadPool&);
    CTreadPool& operator =(CTreadPool&);
    //检测线程池中的线程是否运行超时，线程全忙时增加线程
    void CheckPool();
    void ActiveFreeThread();
    void ReleaseAllThread(mapThreadPool_t & ThreadPool);
private:
    mapThreadPool_t m_FreeThreadArray;
    mapThreadPool_t m_BusyThreadArray;
    void* m_Data_Pool[MAX_DATA_NODE_NUM];
    unsigned int m_CurNodeNum;
    Mutex m_Mutex;
    bool m_bInitalFlag;
};

#endif  // _socklib_H_
