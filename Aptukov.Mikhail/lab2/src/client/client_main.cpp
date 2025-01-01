#include "client.hpp"

#ifdef FIFO
ConnectionType type = ConnectionType::Fifo;
#elif defined(MQ)
ConnectionType type = ConnectionType::Mq;
#elif defined(SOCKET)
ConnectionType type = ConnectionType::Socket;
#endif

void client_signal_handler(int sig, siginfo_t *info, void *context)
{
    switch (sig)
    {
    case SIGUSR1:
        Client::get_instance(type, std::filesystem::current_path() / "host/host.txt").process_chat();
        break;
    case SIGUSR2:
        Client::get_instance(type, std::filesystem::current_path() / "host/host.txt").process_general_chat();
        break;
    }
}

void ClientMainWindow::sendMessageToGeneralChat(const std::string &msg)
{
    Client::get_instance(type, std::filesystem::current_path() / "host/host.txt").send_to_host_general(msg);
}

void ClientMainWindow::sendMessageToChat(const std::string &msg)
{
    Client::get_instance(type, std::filesystem::current_path() / "host/host.txt").send_to_host(msg);
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    ClientMainWindow window;
    window.show();
    Client::get_instance(type, std::filesystem::current_path() / "host/host.txt").set_up_ui(&window);
    
    return app.exec();
}