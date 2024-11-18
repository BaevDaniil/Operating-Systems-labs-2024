#include "shm_names.hpp"

using namespace host_namespace;

namespace
{
    TempHost host = TempHost::get_instance(host_pid_path, identifier);
    int demo_client_pid; // only for demo and testing
}

void host_signal_handler(int sig, siginfo_t *info, void *context)
{
    std::cout << "signal was handled" << std::endl;
    std::cout << "pid:" << info->si_pid << std::endl;
    if (!host.table.contains(info->si_pid))
    {
        host.table.emplace(info->si_pid, TempClientInfo{getpid(), info->si_pid, identifier});
        std::cout << "emplaced" << std::endl;
        host.table[info->si_pid].start();
        std::cout << "started" << std::endl;
        demo_client_pid = info->si_pid;
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
        msg.clear();
        break;
    case SIGUSR2:
        f = host.table[info->si_pid].read_from_client_general(general_msg);
        std::cout << general_msg << "\nstatus: " << f << std::endl;
        host.send_message_to_all_clients_except_one(general_msg, info->si_pid);
        general_msg.clear();
        break;
    default:
        break;
    }
}

int main()
{
    std::cout << getpid() << std::endl;
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        host.push_message(demo_client_pid , "host_abc");
        std::cout << demo_client_pid << " : push_message" << std::endl;
    }
    return 0;
}