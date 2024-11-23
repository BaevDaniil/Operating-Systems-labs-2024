#ifndef GOAT_HANDLER_H
#define GOAT_HANDLER_H

#include "../utilsConfigurations/gameWorld.h"
#include "../connections/connection.h"


class goatHandler 
{
public:
    goatHandler() = default;

    goatHandler(const goatHandler&) = default;
    goatHandler& operator=(const goatHandler&) = default;

    goatHandler (goatHandler&&) = default;
    goatHandler& operator = (goatHandler&&) = default;

    ~goatHandler(){
        syslog(LOG_INFO, "Delete goat Handler");
    }

    bool initGoat(pid_t goatPid);

    void runRounds();

    void stop();
private:
    const int timeInter = 5;

    std::atomic<pid_t> hostPid = -1;
    std::atomic<pid_t> goatPid = -1;
    std::atomic<bool> isOpenSem = false;

    sem_t* hostSemaphore;
    sem_t* clientSemaphore;

    std::unique_ptr<Connection> connect;
    std::shared_ptr<GameWorld> gameWorld;

    bool openSemaphores();
    bool openConnection();
}

#endif