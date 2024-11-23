#include "goatHandler.h"


bool goatHandler::initGoat(pid_t goatPid, std::shared_ptr<GameWorld> gameW)
{
    hostPid = getpid();

    gameWorld = gameW;
    auto goat = std::make_shared<Goat>();
    gameWorld->goatMap.set(goatPid, goat);

    if (!openSemaphores()){
        gameWorld->goatMap.get(goatPid)->stateClient = DISCONNECTION;
        isOpenSem = false;
        return true;
    };
    
    if (!openConnection()){
        gameWorld->goatMap.get(goatPid)->stateClient = DISCONNECTION;
        isOpenSem = false;
        return true;
    }

    isOpenSem = true;
    syslog(LOG_INFO, "Connect goat: %d", int(goatPid));
    gameWorld->goatMap.get(goatPid)->stateClient = CONNECTION;
    gameWorld->goatMap.get(goatPid)->status = ALIVE;
    gameWorld->aliveGoatNumber++;
    return true;
}


bool goatHandler::openSemaphores(){
    std::string semNameHost = semWolfName + std::to_string(hostPid) + std::to_string(goatPid);
    hostSemaphore = sem_open(semNameHost.c_str(), O_CREAT | O_EXCL, 0777, 0);
    if (hostSemaphore == SEM_FAILED) {
        syslog(LOG_ERR, "Semaphore creation error %s", semNameHost.c_str());
        return false;
    }

    std::string semNameClient = semGoatName + std::to_string(hostPid) + std::to_string(goatPid);
    clientSemaphore = sem_open(semNameClient.c_str(), O_CREAT | O_EXCL, 0777, 0);
    if (clientSemaphore == SEM_FAILED) {
        syslog(LOG_ERR, "Semaphore creation error %s", semNameClient.c_str());
        sem_close(hostSemaphore);
        return false;
    }

    sem_post(clientSemaphore);

    syslog(LOG_INFO, "Semaphore created %s %s", semNameHost.c_str(), semNameClient.c_str());
    return true;
}

bool goatHandler::openConnection(){
    syslog(LOG_INFO, "Creating connection for goat %d", goatPid);
    try {
        connect = std::make_unique<Connection>(Conn::GetConn(hostPid, Conn::Type::HOST));
    }
    catch (std::exception &e) {
        syslog(LOG_ERR, "Connection creation error for goat %d", int(goatPid));
        goatPid = -1;
        return false;
    }
    catch (const char *e) {
        syslog(LOG_ERR, "Connection creation error for goat %d", int(goatPid));
        goatPid = -1;
        return false;
    }
    syslog(LOG_INFO, "Connection opened for goat %d", int(goatPid));
    return true;
}

void goatHandler::stop()
{
    sem_close(hostSemaphore);
    sem_close(clientSemaphor);
}
