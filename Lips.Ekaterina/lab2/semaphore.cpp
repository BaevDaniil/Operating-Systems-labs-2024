#include "semaphore.h"


Semaphore::Semaphore(unsigned int value, bool host_flag) {
    std::string semaphore_name = "/sem" + std::to_string(value);
    if (host_flag) {
        semaphore = sem_open(semaphore_name.c_str(), O_CREAT | O_EXCL, 0777, 2);
        if (semaphore == SEM_FAILED) {
            std::cout << "Could not open semaphore --> Terminate\n";
            exit(EXIT_FAILURE);
        }
    }
    else {
        semaphore = sem_open(semaphore_name.c_str(), 0);
        if (semaphore == SEM_FAILED) {
            std::cout << "Could not open semaphore --> Terminate\n";
            exit(EXIT_FAILURE);
        }
    }
}

bool Semaphore::wait() {
    if (sem_wait(semaphore) == -1) {
        std::cout << "Semaphore wait failed" << std::endl;
        return false;
    }
    return true;
}

bool Semaphore::post() {
    if (sem_post(semaphore) == -1) {
        std::cout << "Semaphore post failed" << std::endl;
        return false;
    }
    return true;
}

Semaphore::~Semaphore() {
    if (sem_destroy(semaphore) == -1) {
        std::cout << "Semaphore destruction failed" << std::endl;
    }
}
