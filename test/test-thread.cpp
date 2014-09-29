/**
 * ¼ÇÂ¼ÈÕÖ¾
 *Author:
 *   wanghui5257@gmail.com
 *Copyright:
 *   Copyright(C) 2014.09
 **/
#include <unistd.h>
#include "log.h"
#include "test-thread.h"
TestThread::TestThread():CThread()
{

}
TestThread::~TestThread()
{

}
void TestThread::Run()
{
    trace_log(DBG, "****run start****");
    void* pData = m_pCTreadPool->PopData();
    trace_log(DBG, "process data:%s", (char*)pData);
    delete [] (char*)pData;
    sleep(1);
    trace_log(DBG, "****run end****");

}
CThread* TestThreadPool::CreateThread()
{
    CThread* p = new TestThread();
    return p;
}
