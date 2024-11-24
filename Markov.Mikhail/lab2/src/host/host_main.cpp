#ifdef USE_FIFO_FILE
#include "fifo_names.hpp"
#elif defined(USE_MQ_FILE)
#include "mq_names.hpp"
#elif defined(USE_SHM_FILE)
#include "shm_names.hpp"
#endif
#include "host_gui.hpp"

using namespace host_namespace;

namespace
{
    TempHost& host = TempHost::get_instance(host_pid_path, identifier);
    MainWindow *mainwindow_pointer;
}

void host_signal_handler(int sig, siginfo_t *info, void *context)
{
    if (!mainwindow_pointer)
        return;
    if (!host.table.contains(info->si_pid))
    {
        host.table.emplace(info->si_pid, TempClientInfo{getpid(), info->si_pid, identifier});
        auto f = [](MainWindow *mainwindow_pointer, int pid, const std::string &msg)
        {
            mainwindow_pointer->set_msg_to_chat(pid, msg);
        };
        host.table[info->si_pid].start(mainwindow_pointer, f);
        mainwindow_pointer->add_client(info->si_pid);
        return;
    }
    std::string general_msg = " ";
    switch (sig)
    {
    case SIGUSR1:
        host.table[info->si_pid].append_unread_counter();
        break;
    case SIGUSR2:
        host.table[info->si_pid].read_from_client_general(general_msg);
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