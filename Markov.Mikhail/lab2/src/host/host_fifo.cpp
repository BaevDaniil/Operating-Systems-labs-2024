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
    try{
    std::cout << "signal was handled" << std::endl;
    std::cout << "pid:" << info->si_pid << std::endl;
    if(!host.table.contains(info->si_pid))
    {
        host.table.emplace(info->si_pid, ClientInfo<NamedChannel>{getpid(), info->si_pid, identifier});
        std::cout << "emplaced" << std::endl;
        host.table[info->si_pid].start();
        std::cout << "started" << std::endl;
        return;
    }
    std::string msg = " ";
    std::string general_msg = " ";
    bool f;
    demo_client_pid = info->si_pid;
    switch (sig)
    {
    case SIGUSR1:

        f = host.table[info->si_pid].read_from_client(msg);
        std::cout << msg <<"\nstatus: " << f<< std::endl;
        msg.clear();
        break;
    case SIGUSR2:
        f = host.table[info->si_pid].read_from_client_general(general_msg);
        std::cout << general_msg <<"\nstatus: " << f << std::endl;
        general_msg.clear();
        break;
    default : 
        break;
    }
    }
    catch(const std::exception& ex)
    {
        std::cout << ex.what() << std::endl;
    }
}

int main()
{
    std::cout << getpid() << std::endl;
    while(true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        if (host.get_table().contains(demo_client_pid))
        {
            std::cout << demo_client_pid << " : pid exist" << std::endl; 
            host.get_table()[demo_client_pid].push_message("host_abc");
            std::cout << demo_client_pid << " : push_message" << std::endl;
        }
    }
    return 0;
}