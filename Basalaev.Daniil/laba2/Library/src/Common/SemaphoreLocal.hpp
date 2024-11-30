#pragma once

#include <semaphore.h>

class SemaphoreLocal
{
public:
    SemaphoreLocal(uint value);
    void wait();
    void post();
    ~SemaphoreLocal();

private:
    sem_t semaphore;
};
