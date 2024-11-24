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


int main(int argc, char *argv[]) 
{
    openlog("lab2_host", LOG_PID | LOG_NDELAY | LOG_PERROR, LOG_USER);

    Host host;
    int status = EXIT_SUCCESS;
    pid_t hostPid = getpid();

    host.init(clientNum);
    if (getpid() == hostPid){
        status = host.runHost();
    }

    syslog(LOG_INFO, "End process %d\n", getpid());
    closelog();
    return status;
}


Host::Host()
{
    struct sigaction sig{};
    memset(&sig, 0, sizeof(sig));
    sig.sa_sigaction = SignalHandler;
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

    syslog("Host pid = %d", int(getpid()));

    gameW = std::make_shared<GameState>();
    gameW->deadGoatNumber = 0;
    gameW->aliveGoatNumber = 0;

}

void Host::SignalHandler(int signum, siginfo_t *si, void *data) {
  switch (signum) {
  case SIGTERM:
      Terminate();
      break;
  case SIGUSR1:
      try
      {
        goatHandler goatH = std::make_shared<goatHandler>();
        bool initStatus = goatH->initGoat(si->si_pid, gameW);
        if (initSatus)
        {
          std::thread clientHandlerThread(&ClientHandler::runRounds, goatH);
          clientHandlerThread.detach();
          goats.set(si->si_pid, goatH);
          syslog(LOG_INFO, "Open goat in thread");
        }
        else
        {
          break;
        }
      }
      catch (std::exception &e) {
            syslog(LOG_ERR, "Error: %s", e.what());
        }
      
  default:
    break;
  }
}

void Host::runHost()
{
    syslog(LOG_INFO, "Host started");

    try {
        std::thread connectionThread(&Host::wolfAndGoatGame, this);

        //int argc = 1;
        //char* args[] = { (char*)"WolfAndGoat" };
        //QApplication app(argc, args);

        //syslog(LOG_INFO, "Create_window");
        //GUI window(_gst);

        gameWorld->time = 3;
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

void Host::Terminate()
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

    while (!Terminated.load())
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
            wolfInstance.getRandomNumber();
        }
        gameW->wolfNumber = wolfInstance.wolfNumber.load();

        safeVector<pid_t> keysGoat = goats.getAllKeys();  
        for (size_t i = 0; i < keysGoat.len(); i++)
        {
            goats.get(keysGoat[i]) -> setStatusWolfNumber();
        }

        wolfInstance.isRandomNumber = true;
        std::this_thread::sleep_for(std::chrono::seconds(3));
        gameW->time = 3;
    }

    safeVector<pid_t> keysGoat = goats.getAllKeys();  
    for (size_t i = 0; i < keysGoat.len(); i++)
    {
        syslog(LOG_INFO, "Close goat: %d", int(keysGoat[i]));
        goats.get(keysGoat[i]) -> stop();
    }
}

void Host::Wolf::genRandomNumber()
{
    wolfNumber = int((1.0*std::rand() + 1) / (1.0*RAND_MAX + 1) * (maxNumber - minNumber) + minNumber);
    syslog("WolfNumber = %d", wolfNumber.load())
}