#ifndef HOST_H
#define HOST_H

#include <atomic>
#include <csignal>
#include <sys/stat.h>

#include "../utilsConfigurations/safeMap.h"
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
    
//public slots:
    void wolfGame();

private:
    struct Wolf
    {
        int minNumber=1;
        int maxNumber=100;
        std::atomic<bool> isRandomNumber;
        std::atomic<int> wolfNumber;

        int genRandomNumber();
    }

    Wolf wolfInstance;

    SafeMap<pid_t, goatHandler> goats;
}

#endif
