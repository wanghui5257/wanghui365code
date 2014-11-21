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
	bool InitialPool(unsigned int nThreadNum, unsigned int nOverTime = 60/*default 1 min*/);
    virtual void PushData(void* pDataBuffer);
    virtual void* PopData();
    virtual void Run();
    virtual void ReleaseDataInPool();
    const unsigned int GetTotalThreadNum();
    const unsigned int GetFreeThreadNum();
//void ExitThreadPool();
protected:
    void ReleaseFreeThread(CThread* pThread);
private:
    //forbid copy
    CTreadPool(CTreadPool&);
    CTreadPool& operator =(CTreadPool&);
    void ActiveFreeThread();
    void ReleaseAllThread(mapThreadPool_t & ThreadPool);
    //检测线程池中的线程是否运行超时，线程全忙时增加线程
    static void CheckAllThread(CTreadPool* pThreadPool);
private:
    mapThreadPool_t m_FreeThreadArray;
    mapThreadPool_t m_BusyThreadArray;
    void* m_Data_Pool[MAX_DATA_NODE_NUM];
    unsigned int m_CurNodeNum;
    Mutex m_Mutex;
    bool m_bInitalFlag;
    unsigned int m_nThreadRunOverTime;//秒为单位
};

#endif  // _socklib_H_
