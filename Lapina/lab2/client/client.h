#ifndef CLIENT_H
#define CLIENT_H

#include <unistd.h>
#include <sys/types.h>
#include <semaphore.h>
#include <cstdlib>

#include "../connections/connection.h"
#include "../utilsConfigurations/gameWorld.h"


class Client {
public:
    Client(pid_t hostPid);
    ~Client(void) = default; 
    Client(const Client&) = delete;
    Client& operator=(const Client&) = delete;

    bool init(pid_t hostPid, int seed);
    void run(void);

    static void Terminate(void) noexcept;
    static void signalHandler(int sig);

private:
    const int maxGoatNumAlive = 101;
    const int maxGoatNumDead = 51;
    const int timeInter = 5;

    pid_t hostPid = -1;
    static bool clientTerminated;
    goatStatus state = goatStatus::ALIVE;

    std::unique_ptr<Connection> connect;
    sem_t* hostSemaphore;
    sem_t* clientSemaphore;
    int connTimeOut=5;

    static void clientSignalHandler(int sig);

    bool openConnection(void);

    bool getStatusGoat(void);
    bool sendNumGoat(void);

    void closeConnection(void);

    int randomGoatNumber(int a, int b);
};

#endif