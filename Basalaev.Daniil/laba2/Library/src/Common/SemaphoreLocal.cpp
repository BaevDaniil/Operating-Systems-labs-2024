#include "SemaphoreLocal.hpp"
#include "Logger.hpp"

SemaphoreLocal::SemaphoreLocal(uint amountConnections)
{
    // If PSHARED then share it with other processes
    if (sem_init(&semaphore, /*PSHARED*/1, amountConnections) == -1)
    {
        LOG_ERROR("SemaphoreLocal", "initialization failed");
    }
}

void SemaphoreLocal::wait()
{
    if (sem_wait(&semaphore) == -1)
    {
        LOG_ERROR("SemaphoreLocal", "wait failed");
    }
}

void SemaphoreLocal::post()
{
    if (sem_post(&semaphore) == -1)
    {
        LOG_ERROR("SemaphoreLocal", "post failed");
    }
}

SemaphoreLocal::~SemaphoreLocal()
{
    if (sem_destroy(&semaphore) == -1)
    {
        LOG_ERROR("SemaphoreLocal", "destroy failed");
    }
}
