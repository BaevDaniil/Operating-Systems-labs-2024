#include "host_window.h"
#include "client_window.h"
#include "includes.h"

void HostChatWindow::setup_conn()
{

}

void HostChatWindow::send_msg()
{

}

void HostChatWindow::read_msg()
{

}

int main(int argc, char *argv[])
{

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

            QApplication app(argc, argv);

            std::string host_path = "/tmp/host_fifo" + std::to_string(getppid());
            ClientChatWindow client_window(host_path);
            client_window.show();

            return app.exec();
        }
        else
        {
            pid_clients.push_back(pid);
        }
    }

    std::vector<std::string> clients_path;
    for(auto& pid : pid_clients)
    {
        std::string client_path = "/tmp/client_fifo" + std::to_string(pid);
        clients_path.push_back(client_path);
    }

    QApplication app(argc, argv);
    HostChatWindow host_window(clients_path);
    host_window.show();
    return app.exec();
}
