/**
 * ¼ÇÂ¼ÈÕÖ¾
 *Author:
 *   wanghui5257@gmail.com
 *Copyright:
 *   Copyright(C) 2014.09
 **/
#include <unistd.h>
#include <assert.h>
#include <time.h>
#include "test-thread.h"
#include "log.h"
#include <string>
#include <iostream>
#include <sstream>
#include <map>
void test1()
{
    trace_log(DBG,"------------------test1-----------------");
    TestThread test;
    test.Start();
    //sleep(2);
}
void test2()
{
    trace_log(DBG,"------------------test2-----------------");
    TestThreadPool tp;
    bool bRet = tp.InitialPool(2);
    //sleep(1);
    assert( bRet == true);
    char* pData = new char[100];
    strcpy(pData, "test thread pool");
    tp.PushData(pData);
    sleep(2);
}
void test3()
{
    trace_log(DBG,"------------------test3-----------------");
    TestThreadPool tp;
    bool bRet = tp.InitialPool(2);
    //sleep(1);
    assert( bRet == true);
    unsigned int tNum = tp.GetTotalThreadNum();
    assert(tNum == 2);
    char* pData = new char[100];
    strcpy(pData, "test thread pool");
    tp.PushData(pData);
    //sleep(1);
    tNum = tp.GetFreeThreadNum();
    assert(tNum == 1);
    char* pData1 = new char[100];
    strcpy(pData1, "test thread pool");
    tp.PushData(pData1);
    tNum = tp.GetFreeThreadNum();
    assert(tNum == 0);
    sleep(2);
}
int main(int argc, char* argu[])
{

    time_t tempTime = 0;
    time(&tempTime);
    tm* CurTime = localtime(&tempTime);
    trace_log(DBG, "time: %ld", tempTime);
    trace_log(DBG, "%4d-%02d-%2d ", CurTime->tm_year + 1900, CurTime->tm_mon+1, CurTime->tm_mday);
    test1();
    test2();
    test3();
    //sleep(5);
    time(&tempTime);
    trace_log(DBG, "time: %ld", tempTime);
    sleep(1);
    time(&tempTime);
    trace_log(DBG, "time: %ld", tempTime);
    return 0;
}
