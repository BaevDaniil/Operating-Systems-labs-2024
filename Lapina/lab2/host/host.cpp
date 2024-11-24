#include "host.h"

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

    openlog("lab2_host", LOG_NDELAY | LOG_PID | LOG_PERROR, LOG_USER);

    syslog(LOG_INFO, "Host pid = %d", int(getpid()));

    gameW = std::make_shared<GameWorld>();
    gameW->deadGoatNumder = 0;
    gameW->aliveGoatNumber = 0;

}

int Host::runHost()
{
    syslog(LOG_INFO, "Host started");

    try {
        std::thread connectionThread(&Host::wolfAndGoatGame, this);

        //int argc = 1;
        //char* args[] = { (char*)"WolfAndGoat" };
        //QApplication app(argc, args);

        //syslog(LOG_INFO, "Create_window");
        //GUI window(_gst);

        gameW->time = 3;
        //_timer = std::make_unique<QTimer>(this);
        //QObject::connect(_timer.get(), SIGNAL(timeout()), this, SLOT(updateTimer()));
        //_timer->start(1000);

        //QObject::connect(&window, SIGNAL(wolfNumberSend(int)), this, SLOT(wolfNumberEnter(int)));
        //QObject::connect(this, SIGNAL(gameover()), &window, SLOT(gameover()));
        //window.SetTitle(args[0]);
        //window.show();
        //app.exec();

        //Terminate();
        connectionThread.join();
    }
    catch (std::exception &e) {
        syslog(LOG_ERR, "Error: %s", e.what());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

void Host::GoatStartInit(pid_t goatPid)
{
    try
      {
        std::shared_ptr<goatHandler> goatH = std::make_shared<goatHandler>();
        bool initStatus = goatH->initGoat(goatPid, gameW);
        if (initStatus)
        {
          std::thread goatHandlerThread(&goatHandler::runRounds, goatH);
          goatHandlerThread.detach();
          goats.set(goatPid, goatH);
          syslog(LOG_INFO, "Open goat in thread");
        }
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
    }
}

void Host::wolfAndGoatGame()
{
    syslog(LOG_INFO, "host pid = %d", int(getpid()));
    std::srand(std::time(0));

    bool allDied = false;
    std::this_thread::sleep_for(std::chrono::seconds(120));

    while (!hostTerminated.load())
    {
        if (gameW->aliveGoatNumber.load() == 0){
            if (allDied == true)
            {
                syslog(LOG_INFO, "Gameover");
                //emit gameover();
                std::this_thread::sleep_for(std::chrono::seconds(3));
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

        std::vector<pid_t> keysGoat = goats.getAllKeys();  

        std::mutex mut1;
        std::unique_lock<std::mutex> lk(mut1);
        for (size_t i = 0; i < keysGoat.size(); i++)
        {
            goats.get(keysGoat[i]) -> setStatusWolfNumber();
        }
        lk.unlock();

        wolfInstance.isRandomNumber = true;
        std::this_thread::sleep_for(std::chrono::seconds(3));
        gameW->time = 3;
    }

    std::vector<pid_t> keysGoat = goats.getAllKeys();  
    std::mutex mut2;
    std::unique_lock<std::mutex> lk2(mut2);
    for (size_t i = 0; i < keysGoat.size(); i++)
    {
        syslog(LOG_INFO, "Close goat: %d", int(keysGoat[i]));
        goats.get(keysGoat[i]) -> stopGoat();
    }
    lk2.unlock();
}

void Host::Wolf::genRandomNumber()
{
    wolfNumber = int((1.0*std::rand() + 1) / (1.0*RAND_MAX + 1) * (maxNumber - minNumber) + minNumber);
    syslog(LOG_INFO, "WolfNumber = %d", wolfNumber.load());
}