#include "host.h"
#include "../gui/guiHost.h"

#include <sys/syslog.h>
#include <thread>
#include <csignal>
#include <chrono>
#include <ctime>
#include <unistd.h>
#include <semaphore.h>
#include <cstring>
#include <fcntl.h>
#include <vector>
#include <condition_variable>
#include <mutex>


Host Host::instance;
Host* Host::GetInstance() {
    return &instance;
}

int main(int argc, char *argv[]) 
{
    openlog("lab2_host", LOG_PID | LOG_NDELAY | LOG_PERROR, LOG_USER);

    int status = EXIT_SUCCESS;
    pid_t hostPid = getpid();

    if (getpid() == hostPid){
        status = Host::GetInstance()->runHost();
    }

    syslog(LOG_INFO, "End process %d\n", hostPid);
    closelog();
    return status;
}

void Host::HostSignalHandler(int signum, siginfo_t *si, void *data) {
  switch (signum) {
  case SIGTERM:
      Host::GetInstance()->Terminate();
      break;
  case SIGUSR1:
      Host::GetInstance()->GoatStartInit(si->si_pid);
      break;
  default:
    break;
  }
}

Host::Host()
{
    struct sigaction sig{};
    memset(&sig, 0, sizeof(sig));
    sig.sa_sigaction = HostSignalHandler;
    sig.sa_flags = SA_SIGINFO;

    if (sigaction(SIGTERM, &sig, nullptr)==-1)
    {
        throw std::runtime_error("Failed to register SIGTERM");
    }
    if (sigaction(SIGUSR1, &sig, nullptr)==-1)
    {
        throw std::runtime_error("Failed to register SIGUSR1");
    }

    syslog(LOG_INFO, "Host pid = %d", int(getpid()));

    gameW = std::make_shared<GameWorld>();
    gameW->deadGoatNumder = 0;
    gameW->aliveGoatNumber = 0;

}

int Host::runHost()
{
    syslog(LOG_INFO, "Host started");

    try 
    {   

        std::thread connectionThread(&Host::wolfAndGoatGame, this);

        int argc = 1;
        char* args[] = { (char*)"WolfAndGoat" };
        QApplication app(argc, args);

        syslog(LOG_INFO, "Create_window");
        guiHost window(gameW, int(getpid()));

        gameW->time = 3;
        wolfTimer = std::make_unique<QTimer>(this);
        QObject::connect(wolfTimer.get(), SIGNAL(timeout()), this, SLOT(updateWolfTimer()));

        QObject::connect(&window, SIGNAL(countGoatsSend(int)), this, SLOT(countGoats(int)));
        QObject::connect(this, SIGNAL(countRemainsGoats(int)), &window, SLOT(countGoatsRSend(int)));
        QObject::connect(this, SIGNAL(connectionGoatLog(std::string)), &window, SLOT(writeLog(std::string)));
        QObject::connect(&window, SIGNAL(wolfNumberSend(int)), this, SLOT(wolfNumberEnter(int)));
        QObject::connect(this, SIGNAL(setGameOver()), &window, SLOT(gameover()));
        
        window.show();
        app.exec();

        Terminate();
        connectionThread.join();
    }
    catch (std::exception &e) {
        syslog(LOG_ERR, "Error: %s", e.what());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

void Host::countGoats(int countGoats)
{
    GoatsNumber=countGoats;
    cv.notify_one();
}

void Host::wolfNumberEnter(int number){
    wolfInstance.isRandomNumber = false;
    wolfInstance.wolfNumber = number;
}

void Host::updateWolfTimer(){
    gameW->time--;
}

void Host::GoatStartInit(pid_t goatPid)
{
    try
      {
        if (int(goats.len())==GoatsNumber)
        {
            syslog(LOG_INFO, "All goats in game. Wait next game");
            kill(goatPid, SIGTERM);
            return;
        }

        std::shared_ptr<goatHandler> goatH = std::make_shared<goatHandler>();
        bool initStatus = goatH->initGoat(goatPid, gameW);
        if (initStatus)
        {
          std::thread goatHandlerThread(&goatHandler::runRounds, goatH);
          goatHandlerThread.detach();
          goats.set(goatPid, goatH);
          syslog(LOG_INFO, "Open goat in thread");
          emit connectionGoatLog("Connect goat: " + std::to_string(int(goatPid)));
        }
        cv.notify_one();
      }
      catch (std::exception &e) {
            syslog(LOG_ERR, "Error: %s", e.what());
        }
}

void Host::Terminate() noexcept
{
    if (!hostTerminated.load())
    {
        hostTerminated=true;
        syslog(LOG_INFO, "Terminating host");
        
        std::vector<pid_t> keysGoat = goats.getAllKeys(); 
        std::mutex mut2;
        mut2.lock();
        for (size_t i = 0; i < keysGoat.size(); i++)
        {
            syslog(LOG_INFO, "Close goat: %d", int(keysGoat[i]));
            emit connectionGoatLog("Close goat: " + std::to_string(int(keysGoat[i])));
            goats.get(keysGoat[i]) -> stopGoat();
        }
        mut2.unlock();
    }
}

void Host::wolfAndGoatGame()
{
    syslog(LOG_INFO, "host pid = %d", int(getpid()));
    std::srand(std::time(0));

    bool allDied = false;

    std::mutex mut;
    std::unique_lock lock(mut);
    while(GoatsNumber<0)
    {
        syslog(LOG_INFO, "Wait goats count");
        cv.wait(lock);
    }
    lock.unlock();

    std::mutex m;
    std::unique_lock lk(m);
    while(int(goats.len())!=GoatsNumber)
    {
        emit countRemainsGoats(GoatsNumber-int(goats.len()));
        syslog(LOG_INFO, "Wait goats: %d", GoatsNumber-int(goats.len()));
        cv.wait(lk);
    }
    lk.unlock();
    emit connectionGoatLog("All goat connections");
    emit countRemainsGoats(GoatsNumber-int(goats.len()));
    wolfTimer->start(360);
    std::this_thread::sleep_for(std::chrono::seconds(6));

    std::vector<pid_t> keysGoat = goats.getAllKeys();  

    std::mutex mut1;
    mut1.lock();
    for (size_t i = 0; i < keysGoat.size(); i++)
    {
        goats.get(keysGoat[i]) -> sendStartGame();
    }
    mut1.unlock();


    while (!hostTerminated.load())
    {
        if (gameW->aliveGoatNumber.load() == 0){
            if (allDied == true)
            {
                syslog(LOG_INFO, "Gameover");
                emit setGameOver();
                Terminate();
                continue;
            }

            allDied = true;
        }
        else
        {
            allDied = false;
        }

        if (wolfInstance.isRandomNumber.load()){
            wolfInstance.genRandomNumber();
        }
        gameW->wolfNumber = wolfInstance.wolfNumber.load();
        syslog(LOG_INFO, "WolfNumber = %d", gameW->wolfNumber.load());

        std::mutex mut1;
        mut1.lock();
        for (size_t i = 0; i < keysGoat.size(); i++)
        {
            goats.get(keysGoat[i]) -> setStatusWolfNumber();
        }
        mut1.unlock();

        wolfInstance.isRandomNumber = true;
        std::this_thread::sleep_for(std::chrono::seconds(3));
        gameW->time = 3;
    }
}

void Host::Wolf::genRandomNumber()
{
    wolfNumber = int((1.0*std::rand() + 1) / (1.0*RAND_MAX + 1) * (maxNumber - minNumber) + minNumber);
}