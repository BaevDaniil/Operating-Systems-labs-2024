#include "goatHandler.h"

#include <cstdlib>
#include <semaphore.h>
#include <sys/syslog.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <string>
#include <thread>
#include <csignal>

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
    std::string semNameHost = "/hostWolf" + std::to_string(hostPid) + std::to_string(goatPid);
    hostSemaphore = sem_open(semNameHost.c_str(), O_CREAT | O_EXCL, 0777, 0);
    if (hostSemaphore == SEM_FAILED) {
        syslog(LOG_ERR, "Semaphore creation error %s", semNameHost.c_str());
        return false;
    }

    std::string semNameClient = "/clientGoat" + std::to_string(hostPid) + std::to_string(goatPid);
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
    syslog(LOG_INFO, "Creating connection for goat %d", int(goatPid));
    try {
        connect = Connection::GetConn(hostPid, goatPid, Connection::Type::HOST);
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

void goatHandler::stopGoat()
{
    sem_close(hostSemaphore);
    sem_close(clientSemaphore);
    isOpenSem=false;

    if (goatPid != -1)
    {
        kill(goatPid, SIGTERM);
    }
}


bool goatHandler::sendGoatStatus(goatStatus stGoat)
{
    if (connect->Write(&stGoat, sizeof(stGoat))) {
        sem_post(clientSemaphore);
        return true;
    }
    else {
        syslog(LOG_ERR, "Connection writing error");
        gameWorld->goatMap.get(goatPid)->stateClient = DISCONNECTION;
        kill(goatPid, SIGTERM);
        if(gameWorld->goatMap.get(goatPid)->status == ALIVE)
            gameWorld->aliveGoatNumber--;
        else
            gameWorld->deadGoatNumder--;
        return false;
    }
    return false;
}

bool goatHandler::getGoatNum(int* num)
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += timeInter;
    if (sem_timedwait(hostSemaphore, &ts) == -1)
    {
        syslog(LOG_ERR, "Read semaphore timeout");
        gameWorld->goatMap.get(goatPid)->stateClient = DISCONNECTION;
        kill(goatPid, SIGTERM);
        if(gameWorld->goatMap.get(goatPid)->status == ALIVE)
            gameWorld->aliveGoatNumber--;
        else
            gameWorld->deadGoatNumder--;
        return false;
    }

    if (!(connect->Read(num, sizeof(num))))
    {
        syslog(LOG_ERR, "Connection reading error");
        gameWorld->goatMap.get(goatPid)->stateClient = DISCONNECTION;
        kill(goatPid, SIGTERM);
        if(gameWorld->goatMap.get(goatPid)->status == ALIVE)
            gameWorld->aliveGoatNumber--;
        else
            gameWorld->deadGoatNumder--;
        return false;
    }

    return true;
}

void goatHandler::runRounds()
{
     while (isOpenSem){
        if (statusWolfNumber.load())
        {
            gameWorld->goatMap.get(goatPid)->stateClient = LOAD;

            int hostNum = gameWorld->wolfNumber.load();
            
            int clientNum;
            if (!getGoatNum(&clientNum)) {
                break;
            }

            int diffForAlive = int(70 / gameWorld->goatMap.len());
            int diffForDead = int(20 / gameWorld->goatMap.len());

            if (gameWorld->goatMap.get(goatPid)->status.load() == goatStatus::ALIVE && abs(clientNum - hostNum) > diffForAlive) 
            {
                gameWorld->goatMap.get(goatPid)->status = goatStatus::DEAD;
                gameWorld->aliveGoatNumber--;
                gameWorld->deadGoatNumder++;
                syslog(LOG_INFO,"Goat dead %d", int(goatPid));
            }
            else if (gameWorld->goatMap.get(goatPid)->status.load() == goatStatus::DEAD && abs(clientNum - hostNum) <= diffForDead) {
                gameWorld->goatMap.get(goatPid)->status = goatStatus::ALIVE;
                gameWorld->aliveGoatNumber++;
                gameWorld->deadGoatNumder--;
                syslog(LOG_INFO,"Goat alive %d", int(goatPid));
            }

            if (!sendGoatStatus(gameWorld->goatMap.get(goatPid)->status.load())) 
            {
                break;
            }

            gameWorld->goatMap.get(goatPid)->stateClient = CONNECTION;
            statusWolfNumber = false;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    syslog(LOG_INFO, "End thread Goat %d", int(goatPid));
}

void goatHandler::setStatusWolfNumber()
{
    statusWolfNumber = true;
}