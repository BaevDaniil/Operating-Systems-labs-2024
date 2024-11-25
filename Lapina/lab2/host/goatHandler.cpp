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
#include <sys/syslog.h>

bool goatHandler::initGoat(pid_t goatPid, std::shared_ptr<GameWorld> gameW)
{
    this->hostPid = getpid();
    this->goatPid = goatPid;

    gameWorld = gameW;
    auto goat = std::make_shared<Goat>();
    

    if (!openSemaphores()){
        kill(goatPid, SIGTERM);
        return true;
    };
    
    if (!openConnection()){
        kill(goatPid, SIGTERM);
        return true;
    }

    gameWorld->goatMap.set(goatPid, goat);
    isOpenSem = true;
    syslog(LOG_INFO, "Connect goat: %d", int(goatPid));
    gameWorld->goatMap.get(goatPid)->stateClient = CONNECTION;
    gameWorld->goatMap.get(goatPid)->status = ALIVE;
    gameWorld->aliveGoatNumber++;
    return true;
}


bool goatHandler::openSemaphores()
{
    std::string semNameHost = "/hostWolf" + std::to_string(hostPid) + std::to_string(goatPid);
    errno=0;
    hostSemaphore = sem_open(semNameHost.c_str(), 0);
    if (hostSemaphore == SEM_FAILED) {
        syslog(LOG_ERR, "Semaphore creation error %s %d", semNameHost.c_str(), errno);
        return false;
    }

    std::string semNameClient = "/clientGoat" + std::to_string(hostPid) + std::to_string(goatPid);
    clientSemaphore = sem_open(semNameClient.c_str(), 0);
    if (clientSemaphore == SEM_FAILED) {
        syslog(LOG_ERR, "Semaphore creation error %s", semNameClient.c_str());
        sem_close(hostSemaphore);
        return false;
    }

    sem_post(hostSemaphore);

    syslog(LOG_INFO, "Semaphore opened %s %s", semNameHost.c_str(), semNameClient.c_str());
    return true;
}

bool goatHandler::openConnection(){
    syslog(LOG_INFO, "Creating connection for goat %d", int(goatPid));
    try {
        connect = Connection::GetConn(hostPid, goatPid, Connection::Type::HOST);
        connect->Open();
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
    if (goatPid != -1)
    {
        kill(goatPid, SIGTERM);
    }
    
    sem_close(clientSemaphore);
    sem_close(hostSemaphore);
    isOpenSem=false;
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

void goatHandler::sendStartGame()
{
    sem_post(clientSemaphore);
}

void goatHandler::runRounds()
{
     while (isOpenSem){
        if (statusWolfNumber.load())
        {

            int hostNum = gameWorld->wolfNumber.load();

            int goatNum;
            if (!getGoatNum(&goatNum)) {
                break;
            }
            
            gameWorld->goatMap.get(goatPid)->goatNumber=goatNum;

            int diffForAlive = int(70 / gameWorld->goatMap.len());
            int diffForDead = int(20 / gameWorld->goatMap.len());

            if (gameWorld->goatMap.get(goatPid)->status.load() == goatStatus::ALIVE && abs(goatNum - hostNum) > diffForAlive) 
            {
                gameWorld->goatMap.get(goatPid)->status = goatStatus::DEAD;
                gameWorld->aliveGoatNumber--;
                gameWorld->deadGoatNumder++;
                syslog(LOG_INFO,"Goat dead %d", int(goatPid));
            }
            else if (gameWorld->goatMap.get(goatPid)->status.load() == goatStatus::DEAD && abs(goatNum - hostNum) <= diffForDead) {
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