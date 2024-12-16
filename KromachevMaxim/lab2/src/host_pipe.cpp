#include "host_window.h"
#include "client_window.h"
#include "includes.h"
#include "conn_pipe.h"

void HostChatWindow::setup_conn()
{
}

void HostChatWindow::send_public_msg(const std::string& msg, __pid_t pid)
{

}

void HostChatWindow::send_private_msg(const std::string& msg)
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

    std::vector<__pid_t> pid_clients;

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
            ClientChatWindow client_window(getppid());
            client_window.show();

            return app.exec();
        }
        else
        {
            pid_clients.push_back(pid);
        }
    }

    QApplication app(argc, argv);
    HostChatWindow host_window(pid_clients);
    host_window.show();
    return app.exec();
}
