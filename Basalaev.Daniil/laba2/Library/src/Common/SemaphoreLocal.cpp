#include "SemaphoreLocal.hpp"
#include "Logger.hpp"

SemaphoreLocal::SemaphoreLocal(uint value)
{
    if (sem_init(&semaphore, 0, value) == -1)
    {
        Logger::getInstance().logError("Semaphore", "Initialization failed");
    }
}

void SemaphoreLocal::wait()
{
    if (sem_wait(&semaphore) == -1)
    {
        Logger::getInstance().logError("Semaphore", "Wait failed");
    }
}

void SemaphoreLocal::post()
{
    if (sem_post(&semaphore) == -1)
    {
        Logger::getInstance().logError("Semaphore", "post failed");
    }
}

SemaphoreLocal::~SemaphoreLocal()
{
    if (sem_destroy(&semaphore) == -1)
    {
        Logger::getInstance().logError("Semaphore", "destroy failed");
    }
}
