#ifndef HOST_H
#define HOST_H

#include <atomic>
#include <csignal>
#include <sys/stat.h>
#include <memory>

#include "../utilsConfigurations/safeMap.h"
#include "../utilsConfigurations/safeVector.h"
#include "../utilsConfigurations/gameWorld.h"
#include "goatHandler.h"

class HOST_H
{
public:
    Host();
    Host(const Host&)=delete;
    Host& operator=(const Host&)=delete;
    Host(const Host&&)=delete;
    Host&& operator=(const Host&&)=delete;
    ~Host()=default;

    std::atomic<bool> hostTerminated=false;

    void HostSignalHandler(int signum, siginfo_t *si, void *data);
    void Terminate() noexcept;
    void runHost();
    
//public slots:
    void wolfAndGoatGame();

private:
    struct Wolf
    {
        int minNumber=1;
        int maxNumber=101;
        std::atomic<bool> isRandomNumber = true;
        std::atomic<int> wolfNumber;

        void genRandomNumber();
    }

    Wolf wolfInstance;
    std::shared_ptr<GameWorld> gameW;
    SafeMap<pid_t, std::shared_ptr<goatHandler>> goats;
}

#endif
