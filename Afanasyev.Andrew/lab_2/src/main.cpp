#include "host/host.hpp"
#include "client/client.hpp"

#ifdef USE_FIFO_FILE
ConnectionType type = ConnectionType::Fifo;
#elif defined(USE_MQ_FILE)
ConnectionType type = ConnectionType::Mq;
#elif defined(USE_SOCKET_FILE)
ConnectionType type = ConnectionType::Socket;
#endif

namespace
{
    int host_pid;
    int client_pid;
}
void host_signal_handler(int sig, siginfo_t *info, void *context)
{
    switch (sig)
    {
    case SIGUSR1:
        Host::get_instance(type, host_pid, client_pid).process_chat();
        break;
    }
}

void client_signal_handler(int sig, siginfo_t *info, void *context)
{
    switch (sig)
    {
    case SIGUSR1:
        Client::get_instance(type, host_pid, client_pid).process_chat();
        break;
    }
}

void MainWindow::send_msg_to_chat(const std::string &msg)
{
    if(is_host)
        Host::get_instance(type, host_pid, client_pid).send_to_client(msg);
    else
        Client::get_instance(type, host_pid, client_pid).send_to_host(msg);
}

int main(int argc, char *argv[])
{

    host_pid = getpid();
    client_pid = fork(); 

    if (client_pid < 0)
    {
        std::cerr << "Fork failed!" << std::endl;
        return 1;
    }
    else if (client_pid == 0)
    {
        QApplication app(argc, argv);

        MainWindow window("client ", false);
        window.show();
        Client::get_instance(type, host_pid, getpid()).set_up_ui(&window);

        return app.exec();
    }
    else
    {
        QApplication app(argc, argv);

        MainWindow window("host ", true);
        window.show();
        Host::get_instance(type, host_pid, client_pid).set_up_ui(&window);

        return app.exec();
    }
}