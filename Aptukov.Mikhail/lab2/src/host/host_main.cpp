#include "host.hpp"

#ifdef FIFO
ConnectionType type = ConnectionType::Fifo;
#elif defined(MQ)
ConnectionType type = ConnectionType::Mq;
#elif defined(SOCKET)
ConnectionType type = ConnectionType::Socket;
#endif

void host_signal_handler(int sig, siginfo_t *info, void *context)
{

    bool f = Host::get_instance(type, std::filesystem::current_path() / "host/host.txt").create_new_client(info->si_pid);
    if(f)
        return;
    switch (sig)
    {
    case SIGUSR1:
        Host::get_instance(type, std::filesystem::current_path() / "host/host.txt").process_chat(info->si_pid);
        break;
    case SIGUSR2:
        Host::get_instance(type, std::filesystem::current_path() / "host/host.txt").process_general_chat(info->si_pid);
        break;
    default:
        break;
    }
}

void MainWindow::send_msg_to_all_clients(const std::string &msg)
{
    std::vector<int> invalid_pids = Host::get_instance(type, std::filesystem::current_path() / "host/host.txt").push_message_to_all_clients(msg);
    for (int i : invalid_pids)
        remove_client(i);
}

void ChatWindow::send_msg(const std::string &msg)
{
    set_text("my: " + msg);
    bool f = Host::get_instance(type, std::filesystem::current_path() / "host/host.txt").push_message(this->pid, msg);
    if (!f)
        valid = false;
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainWindow window;
    window.show();
    Host::get_instance(type, std::filesystem::current_path() / "host/host.txt").set_up_ui(&window);

    return app.exec();
}