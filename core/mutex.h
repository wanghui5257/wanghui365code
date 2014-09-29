#ifndef __MUTEX_H__
#define __MUTEX_H__
#include <pthread.h>
class Mutex
{
public:
    Mutex();
    virtual ~Mutex();
    virtual void lock();
    virtual void unlock();
private:
    Mutex (const Mutex&);
    Mutex& operator= (const Mutex&);
private:
    mutable pthread_mutex_t mId;
    pthread_mutex_t* getId() const;
};

#endif
