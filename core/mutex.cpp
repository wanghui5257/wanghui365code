#include <cassert>
#include <cerrno>
#include "mutex.h"

Mutex::Mutex()
{
    int rc = pthread_mutex_init(&mId, 0);
    assert(rc == 0);
}

Mutex::~Mutex()
{
    int rc = pthread_mutex_destroy(&mId);
    assert(rc == 0);
}

void Mutex::lock()
{
    int rc = pthread_mutex_lock(&mId);
    assert(rc == 0);
}

void Mutex::unlock()
{
    int rc = pthread_mutex_unlock(&mId);
    assert(rc == 0);
}

pthread_mutex_t* Mutex::getId() const
{
    return (&mId);
}
