#include "client.h"

#include <sys/syslog.h>
#include <csignal>
#include <ctime>
#include <thread>
#include <chrono>
#include <string>
#include <semaphore.h>
#include <fcntl.h>


bool Client::clientTerminated = false;

int main(int argc, char* argv[]) 
{
    openlog("lab2_client", LOG_PID | LOG_NDELAY | LOG_PERROR, LOG_USER);

    pid_t hostPid;
    if (argc != 2) {
        std::cout << "Wrong number of arguments. Host pid required.\n";
        return EXIT_FAILURE;
    }
    try {
        hostPid = std::atoi(argv[1]);
    }
    catch (...) {
        std::cout << "Wrong type of argument. Host pid must be integer number.\n";
        return EXIT_FAILURE;
    }

    Client client(hostPid);
    client.run();

    syslog(LOG_INFO, "End goat %d\n", getpid());
    closelog();

    return EXIT_SUCCESS;
}


Client::Client(pid_t HostPid) 
{
    clientTerminated=false;

    hostPid = HostPid;
    std::signal(SIGTERM, signalHandler);

    syslog(LOG_INFO, "Get connection for %d", int(hostPid));
    try {
        connect = Connection::GetConn(hostPid, getpid(), Connection::Type::CLIENT);
    }
    catch (std::exception &e) {
        syslog(LOG_ERR, "Connection getting error");
    }
    catch (const char *e) {
        syslog(LOG_ERR, "Connection getting error");
    }

    std::string semRead = "/clientGoat" + std::to_string(hostPid) + std::to_string(getpid());
    clientSemaphore = sem_open(semRead.c_str(), O_CREAT | O_EXCL, 0777, 0);
    if (clientSemaphore == SEM_FAILED) {
        syslog(LOG_ERR, "Semaphore creation error %s", semRead.c_str());
    }

    std::string semWrite = "/hostWolf" + std::to_string(hostPid) + std::to_string(getpid());
    hostSemaphore = sem_open(semWrite.c_str(), O_CREAT | O_EXCL, 0777, 0);
    if (hostSemaphore == SEM_FAILED) {
        syslog(LOG_ERR, "Semaphore creation error %s", semWrite.c_str());
        sem_close(clientSemaphore);
    }

    syslog(LOG_INFO, "Connection opened");
}

void Client::signalHandler(int sig) {
    switch (sig) {
        case SIGTERM:
            Client::Terminate();
            break;
        default:
            syslog(LOG_ERR, "Unknown signal");
    }
}

void Client::Terminate() noexcept 
{
    if (!clientTerminated) {
        clientTerminated = true;
        syslog(LOG_INFO, "Terminating client");
    }
}

bool Client::openConnection(void) 
{
    time_t t1 = time(nullptr);
    time_t t2;
    bool conn_status = false;
    syslog(LOG_INFO, "Try open connection");
    do {
        conn_status = connect->Open();
        t2 = time(nullptr);
    } while (!conn_status && difftime(t1, t2) < connTimeOut);
    return conn_status;
}

void Client::run(void){
    syslog(LOG_INFO, "Client run goat %d", int(getpid()));

    if (kill(hostPid, SIGUSR1) < 0) {
        syslog(LOG_ERR, "Wrong host pid. Terminate.");
        exit(EXIT_FAILURE);
    }


    if (!openConnection()) {
        syslog(LOG_ERR, "Could not connect host. Terminate");
        exit(EXIT_FAILURE);
    }

    syslog(LOG_INFO, "Start game");
    do {
        if (!sendNumGoat()) {
            syslog(LOG_ERR, "Could not send number. Terminate");
            exit(EXIT_FAILURE);
        }

        if (!getStatusGoat()) {
            syslog(LOG_ERR, "Could not get state. Terminate");
            exit(EXIT_FAILURE);
        }
    } while(state != goatStatus::FINISH || !clientTerminated);

    syslog(LOG_INFO, "Finish goat %d", getpid());
    connect->Close();

    closeConnection();
}

bool Client::getStatusGoat(void)
{
    struct timespec ts;

    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += timeInter;

    if (sem_timedwait(clientSemaphore, &ts) == -1)
        return false;
    bool statusRead = connect->Read(&state, sizeof(state));
    if (statusRead) {
        std::string stateStr = "Alive";
        if (state == goatStatus::DEAD)
            stateStr = "Dead";
        else if (state == goatStatus::FINISH)
            stateStr = "Finish";

        syslog(LOG_INFO, "Goat state is: %s", stateStr.c_str());
    }
    return statusRead;
}

int Client::randomGoatNumber(int a, int b)
{
    return int((1.0*std::rand() + 1) / (1.0*RAND_MAX + 1) * (b - a) + a);
}

bool Client::sendNumGoat(void) 
{
    int num = 0;

    if (state==goatStatus::ALIVE)
    {
        num=randomGoatNumber(1, maxGoatNumAlive);
    }
    else if (state==goatStatus::DEAD)
    {
        num=randomGoatNumber(1, maxGoatNumDead);
    }
    
    syslog(LOG_INFO, "Sent goat %d number is %i", int(getpid()), num);

    bool request = connect->Write(&num, sizeof(num));
    sem_post(hostSemaphore);
    return request;
}

void Client::closeConnection(void) {
    sem_close(clientSemaphore);
    sem_close(hostSemaphore);
}



