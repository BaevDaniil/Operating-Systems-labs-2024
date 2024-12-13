#include "host_/host.hpp"
#include "client_/client.hpp"
#include "gui/client_gui.hpp"
#include "gui/chat_gui.hpp"

#ifdef USE_SHM_FILE
ConnectionType type = ConnectionType::Shm;
#elif defined(USE_MQ_FILE)
ConnectionType type = ConnectionType::Mq;
#elif defined(USE_PIPE_FILE)
ConnectionType type = ConnectionType::Pipe;
#else
ConnectionType type = ConnectionType::Shm;
#endif

namespace {
    int host_pid = -1;
    int client_pid;
}

void host_signal_handler(int sig, siginfo_t *info, void *context) {

    int client_pid = info->si_pid;
    switch (sig)
    {
    case SIGUSR1:
        Host::get_instance(type, host_pid).process_private_chat(client_pid);
        break;
    case SIGUSR2:
        Host::get_instance(type, host_pid).process_general_chat(client_pid);
        break;
    case SIGQUIT:
        Host::get_instance(type, host_pid).remove_client(client_pid);
        break;
    }
}


std::shared_ptr<Client> client_instance;
void client_signal_handler(int sig, siginfo_t *info, void *context) {
    if (client_instance) {
        client_instance->resetIdleTimer();
    }
    else {return;}

    switch (sig)
    {
    case SIGUSR1:
        client_instance->process_host_chat();
        break;
    case SIGUSR2:
        client_instance->process_general_chat();
        break;
    case SIGQUIT:
        client_instance.reset();
        break;
    }
    
}

void MainWindow::send_to_all_clients(const std::string &msg){
    Host::get_instance(type, host_pid).send_to_all_clients(msg);
}

void MainWindow::send_to_client_private(int client_pid, const std::string &msg){
    Host::get_instance(type, host_pid).send_to_client_private(client_pid, msg);
}

const char *SEMAPHORE_NAME = "/host_client_semaphore";

int main(int argc, char *argv[])
{
    if (host_pid == -1) host_pid = getpid();
    static Host& host = Host::get_instance(type, host_pid);

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

    sem_t *semaphore = sem_open(SEMAPHORE_NAME, O_CREAT | O_RDWR, 0666, 0);
    if (semaphore == SEM_FAILED)
    {
        perror("Ошибка создания семафора");
        return 1;
    }

    std::vector<pid_t> pid_clients;
    for (int i = 0; i < clientCount; i++)
    {
        pid_t pid = fork();

        if (pid == -1)
        {
            fprintf(stderr, "Ошибка: функция fork() вернула -1.\n");
            return 1;
        }
        else if (pid == 0)
        {
            sem_t *client_semaphore = sem_open(SEMAPHORE_NAME, O_RDWR);
            if (client_semaphore == SEM_FAILED)
            {
                perror("Ошибка открытия семафора в клиенте");
                return 1;
            }

            sem_wait(client_semaphore);

            int argc = 0;
            char **argv = nullptr;
            client_pid = getpid();
            QApplication clientApp(argc, argv);

            auto client_instance = std::make_shared<Client>(type, host_pid, client_pid, client_signal_handler);
            auto client_window = std::make_shared<ClientMainWindow>(client_pid, client_instance);
            
            // client_instance = std::make_unique<Client>(type, host_pid, client_pid, client_signal_handler);
            client_window->show();
            client_instance->set_up_ui(client_window);
            return clientApp.exec();
        }
        else
        {
            pid_clients.push_back(pid);
        }
    }

    QApplication app_host(argc, argv);
    MainWindow host_window;
    for (auto &pid : pid_clients)
    {
        host.add_client(pid, type);
        host_window.add_client(pid);
    }
    host_window.show();
    host.set_up_ui(&host_window);

    for (int i = 0; i < clientCount; ++i)
    {
        sem_post(semaphore);
    }

    int exit_code = app_host.exec();

    sem_close(semaphore);
    sem_unlink(SEMAPHORE_NAME);

    return exit_code;
}
