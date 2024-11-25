#ifndef HOST_H
#define HOST_H

#include <atomic>
#include <csignal>
#include <sys/stat.h>
#include <memory>
#include <condition_variable>
#include <mutex>

#include <QObject>
#include <QTimer>

#include "../utilsConfigurations/safeMap.h"
#include "../utilsConfigurations/gameWorld.h"
#include "goatHandler.h"

class Host: public QObject
{
    Q_OBJECT
public:
    static Host* GetInstance();

    std::atomic<bool> hostTerminated=false;

    static void HostSignalHandler(int signum, siginfo_t *si, void *data);
    void Terminate() noexcept;
    void GoatStartInit(pid_t goatPid);
    int runHost();

public slots:
    void countGoats(int countGoats);
    void wolfNumberEnter(int number);
    void updateWolfTimer();

signals:
    void countRemainsGoats(int count);
    void connectionGoatLog(const std::string &log_string);
    void openGame();
    void setGameOver();
    
private:
    int GoatsNumber = -1;
    std::condition_variable cv;
    
    struct Wolf
    {
        int minNumber=1;
        int maxNumber=101;
        std::atomic<bool> isRandomNumber = true;
        std::atomic<int> wolfNumber;

        void genRandomNumber();
    };

    Wolf wolfInstance;
    std::shared_ptr<GameWorld> gameW;
    std::unique_ptr<QTimer> wolfTimer;
    SafeMap<pid_t, std::shared_ptr<goatHandler>> goats;     
    void wolfAndGoatGame();

    static Host instance;

    Host();
    Host(const Host&)=delete;
    Host& operator=(const Host&)=delete;
    Host(const Host&&)=delete;
    Host&& operator=(const Host&&)=delete;
    ~Host()=default;
};

#endif
