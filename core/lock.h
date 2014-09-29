#ifndef __LOCK_H__
#define __LOCK_H__
#include "mutex.h"
class Lock
{
public:
    Lock(Mutex &);
    virtual ~Lock();
private:
    Mutex& myMutex;
};


#endif /*_LOCK_H*/
