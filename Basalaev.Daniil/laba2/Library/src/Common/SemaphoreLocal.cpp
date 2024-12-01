#include "SemaphoreLocal.hpp"
#include "Logger.hpp"

SemaphoreLocal::SemaphoreLocal(uint value)
{
    if (sem_init(&semaphore, 0, value) == -1)
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
