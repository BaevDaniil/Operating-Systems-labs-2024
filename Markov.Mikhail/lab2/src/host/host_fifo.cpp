#include "host.hpp"

namespace
{
    using TempHost = Host<NamedChannel>;
    const bool identifier = true;
    const std::filesystem::path host_pid_path = std::filesystem::current_path() / "host/host.txt";
    TempHost host = TempHost::get_instance(host_pid_path, identifier);
    int demo_client_pid;
}

void host_signal_handler(int sig, siginfo_t *info, void *context)
{
    std::cout << "signal was handled" << std::endl;
    if(!host.table.contains(info->si_pid))
    {
        host.table.emplace(info->si_pid, ClientInfo<NamedChannel>{getpid(), info->si_pid, identifier});
        return;
    }
    std::string msg = "a ";
    std::string general_msg = "a ";
    bool f;
    demo_client_pid = info->si_pid;
    switch (sig)
    {
    case SIGUSR1:
        std::cout << info->si_pid << std::endl;
        f = host.table[info->si_pid].read_from_client(msg);
        std::cout << msg <<"\nstatus: " << f<< std::endl;
        msg.clear();
        break;
    case SIGUSR2:
        std::cout << info->si_pid << std::endl;
        f = host.table[info->si_pid].read_from_client_general(general_msg);
        std::cout << general_msg <<"\nstatus: " << f << std::endl;
        general_msg.clear();
        break;
    default : 
        std::cout<< info->si_pid << std::endl;
        break;
    }
}

int main()
{
    std::cout << getpid() << std::endl;
    while(true)
    {
        sleep(1);
    }
    return 0;
}