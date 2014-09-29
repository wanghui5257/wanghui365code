/**
 * ╪гб╪хуж╬
 *Author:
 *   wanghui5257@gmail.com
 *Copyright:
 *   Copyright(C) 2014.09
 **/
#include "thread.h"
#include "threadpool.h"
class CTreadPool;
class TestThread:public CThread
{
public:
    TestThread();
    virtual ~TestThread();
    void Run();
};
class TestThreadPool:public CTreadPool
{
public:
    TestThreadPool(){}
    virtual ~TestThreadPool(){}
    CThread* CreateThread();
};
