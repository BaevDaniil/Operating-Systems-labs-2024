#ifndef CLIENT_H
#define CLIENT_H

#include <unistd.h>
#include <sys/types.h>
#include <semaphore.h>
#include <cstdlib>

#include "../connection/connection.h"
#include "../utilsConfigurations"

class Client {
public:
    struct GoatParameter
    {
        const int maxNumAlive = 100;
        const int maxNumDead = 50;
        const int timeInter = 5;
    }

    Client(pid_t hostPid);
    ~Client(void) = default; 
    Client(const Client&) = delete;
    Client& operator=(const Client&) = delete;

    bool init(pid_t hostPid, int seed);
    void run(void);

    static void terminate(void) noexcept;

private:
    pid_t hostPid = -1;
    static bool clientTerminated = false;
    goatStatus isAlive = goatStatus::ALIVE;

    std::unique_ptr<Connection> connect;
    sem_t* hostSemaphore;
    sem_t* clientSemaphore;

    static void clientSignalHandler(int sig);

    bool openConnection(void);

    bool getStatusGoat(void);
    bool sendNumGoat(void);

    void closeConnection(void);

    int randomGoatNumber(int a, int b);
};

#endif