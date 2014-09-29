#include "lock.h"

Lock::Lock(Mutex &lockable) : myMutex(lockable)
{
    myMutex.lock();
}

Lock::~Lock()
{
    myMutex.unlock();
}

