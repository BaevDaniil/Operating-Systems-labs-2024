#pragma once

#include <semaphore.h>
#include <iostream>
#include <cstring>
#include <fcntl.h>

class Semaphore {
public:
    Semaphore(unsigned int value, bool host_flag);
    bool wait();
    bool post();
    ~Semaphore();

private:
    sem_t* semaphore;
};
