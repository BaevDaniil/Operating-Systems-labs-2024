#include "mq_names.hpp"

#include "host_gui.hpp"

using namespace host_namespace;

namespace
{
    TempHost host = TempHost::get_instance(host_pid_path, identifier);
    bool valid = true;
    MainWindow *mainwindow_pointer;
}

void host_signal_handler(int sig, siginfo_t *info, void *context)
{
    if (sig == SIGINT)
    {
        valid = false;
        return;
    }
    std::cout << "signal was handled" << std::endl;
    std::cout << "pid:" << info->si_pid << std::endl;
    if (!host.table.contains(info->si_pid))
    {
        host.table.emplace(info->si_pid, TempClientInfo{getpid(), info->si_pid, identifier});
        std::cout << "emplaced" << std::endl;
        host.table[info->si_pid].start();
        std::cout << "started" << std::endl;
        mainwindow_pointer->add_client(info->si_pid);
        return;
    }
    std::string msg = " ";
    std::string general_msg = " ";
    bool f;
    switch (sig)
    {
    case SIGUSR1:
        f = host.table[info->si_pid].read_from_client(msg);
        std::cout << msg << "\nstatus: " << f << std::endl;
        mainwindow_pointer->set_msg_to_chat(info->si_pid, msg);
        msg.clear();
        break;
    case SIGUSR2:
        f = host.table[info->si_pid].read_from_client_general(general_msg);
        std::cout << "general: " << general_msg << "\nstatus: " << f << std::endl;
        mainwindow_pointer->set_msg_to_general_chat(general_msg);
        host.send_message_to_all_clients_except_one(general_msg, info->si_pid);
        general_msg.clear();
        break;
    default:
        break;
    }
}

void MainWindow::send_msg_to_all_clients(const std::string &msg)
{
    std::vector<int> invalid_pids = host.push_message_to_all_clients(msg);
    for (int i : invalid_pids)
        remove_client(i);
}

void ChatWindow::send_msg(const std::string &msg)
{
    set_text("my: " + msg);
    bool f = host.push_message(this->pid, msg);
    if (!f)
        valid = false;
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainWindow window;
    mainwindow_pointer = &window;

    window.show();

    return app.exec();
}