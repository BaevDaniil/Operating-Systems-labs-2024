#ifdef FIFO_COMMUNICATION
#include "fifo_names.hpp"
#elif defined(MQ_COMMUNICATION)
#include "mq_names.hpp"
#elif defined(SHM_COMMUNICATION)
#include "shm_names.hpp"
#endif
#include <QApplication>

#include "HostWindow.hpp"
#include "host.hpp"

using namespace host_namespace;

namespace
{
    TempHost& host = TempHost::get_instance(host_pid_path, identifier);
    HostWindow *window_ptr;
}

void host_signal_handler(int sig, siginfo_t *info, void *context)
{
    std::cerr<<"host_signal_handler"<<std::endl;
    if (!window_ptr)
        return;
    if (!host.table.contains(info->si_pid))
    {
        host.table.emplace(info->si_pid, TempClientInfo{getpid(), info->si_pid, identifier});
        return;
    }
    std::string request = " ";

    switch (sig)
    {
        case SIGUSR1:
            std::cerr << "read return request from client" << std::endl;
            host.table[info->si_pid].read_return_request_from_client(request);
            std::cerr<<request<<std::endl;
            window_ptr -> ReturnBook(info->si_pid, request);
            request.clear();
            break;
        case SIGUSR2:
            std::cerr << "read take request from client" << std::endl;
            host.table[info->si_pid].read_take_request_from_client(request);
            std::cerr<<request<<std::endl;
            std::cerr<<"give"<<std::endl;
            window_ptr -> GiveBook(info->si_pid, request);
            request.clear();
            break;
    default:
        break;
    }
}

void HostWindow::send_take_answer_to_client(int client_id, std::string answer) {
    std::cerr<<"send_take_answer_to_client"<<std::endl;
    host.table[client_id].send_take_answer_to_client(answer);
}

void HostWindow::send_return_answer_to_client(int client_id, std::string answer) {
    std::cerr<<"send_return_answer_to_client"<<std::endl;
    host.table[client_id].send_return_answer_to_client(answer);
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    HostWindow window;
    window_ptr = &window;

    window.show();

    return app.exec();
}