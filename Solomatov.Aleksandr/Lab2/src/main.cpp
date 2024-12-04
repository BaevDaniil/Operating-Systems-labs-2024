#include "host_/host.hpp"
#include "client_/client.hpp"

#ifdef USE_SHM_FILE
ConnectionType type = ConnectionType::Shm;
#elif defined(USE_MQ_FILE)
ConnectionType type = ConnectionType::Mq;
#elif defined(USE_PIPE_FILE)
ConnectionType type = ConnectionType::Pipe;
#else
ConnectionType type = ConnectionType::Shm; // Значение по умолчанию
#endif

namespace {
    int host_pid = -1;
    int client_pid;
}

void host_signal_handler(int sig, siginfo_t *info, void *context) {
    if (sig == SIGUSR1) {
        int client_pid = info->si_pid;
        // Обрабатываем сообщения от клиента
        Host::get_instance(type, host_pid).process_chat(client_pid);
    }
}


std::unique_ptr<Client> client_instance;
void client_signal_handler(int sig) {
    if (sig == SIGUSR1 && client_instance) {
        client_instance->resetIdleTimer();
    }
}


int main(int argc, char *argv[])
{
    if (host_pid == -1) host_pid = getpid();

    if (argc < 2)
    {
        fprintf(stderr, "Ошибка: укажите количество клиентов в аргументах командной строки.\n");
        return 1;
    }

    int clientCount = std::stoi(argv[1]);
    if (clientCount <= 0)
    {
        fprintf(stderr, "Ошибка: некорректное количество клиентов.\n");
        return 1;
    }
    
    std::vector<pid_t> pid_clients;

    for(int i = 0; i < clientCount; i++) {

        pid_t pid = fork();

        if (pid == -1)
        {
            fprintf(stderr, "Ошибка: функция fork() вернула -1.\n");
            return 1;

        }
        else if (pid == 0)
        {
            int argc = 0;
            char** argv = nullptr;
            client_pid=pid;
            QApplication clientApp(argc, argv);
            MainWindow clientWindow("Client", false);
            std::signal(SIGUSR1, client_signal_handler);
            auto connection = ConnectionFabric::create_connection(
                type, ConnectionFabric::create_filename(type, host_pid, getpid()), true);
            client_instance = std::make_unique<Client>("", false, std::move(connection));
            client_instance->start();
            clientWindow.show();

            return clientApp.exec();
        }
        else
        {
            pid_clients.push_back(pid);
        }
    }

    QApplication app_host(argc, argv);
    MainWindow host_window("Host", true);
    for(auto& pid : pid_clients)
    {
        host_window.add_client(pid);
    }
    host_window.show();
    return app_host.exec();
}