#ifdef USE_FIFO_FILE
#include "fifo_names.hpp"
#elif defined(USE_MQ_FILE)
#include "mq_names.hpp"
#elif defined(USE_SHM_FILE)
#include "shm_names.hpp"
#endif

#include "client_gui.hpp"

using namespace client_namespace;

namespace
{
    TempClient& client = TempClient::get_instance(host_pid_path, identifier);
    ClientMainWindow *mainwindow_pointer;
}
void client_signal_handler(int sig, siginfo_t *info, void *context)
{
    static std::string msg;
    static std::string msg_general;
    if (!mainwindow_pointer)
        return;
    switch (sig)
    {
    case SIGUSR1:
        client.read_from_host(msg);
        mainwindow_pointer->set_msg_to_chat(msg);
        msg.clear();
        break;
    case SIGUSR2:
        client.read_from_host_general(msg_general);
        mainwindow_pointer->set_msg_to_general_chat(msg_general);
        msg_general.clear();
        break;
    default:
        break;
    }
}

void ClientMainWindow::send_msg_to_general_chat(const std::string &msg)
{
    client.send_to_host_general(msg);
}

void ClientMainWindow::send_msg_to_chat(const std::string &msg)
{
    client.send_to_host(msg);
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    ClientMainWindow window;
    mainwindow_pointer = &window;

    window.show();

    return app.exec();
}