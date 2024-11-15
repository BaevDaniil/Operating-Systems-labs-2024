#include "host.hpp"

namespace
{
    using TempHost = Host<NamedChannel>;
    const bool identifier = true;
    const std::filesystem::path host_pid_path = std::filesystem::current_path() / "host/host.txt";
    TempHost host = TempHost::get_instance(host_pid_path, identifier);
}

inline void host_signal_handler(int sig, siginfo_t *info, void *context)
{
    std::cout << "signal was handled" << std::endl;
    switch (sig)
    {
    case SIGUSR1:
        std::cout << info->si_pid << std::endl;
        if (host.table.contains(info->si_pid))
            host.table.erase(info->si_pid);
        else
            host.table.insert({info->si_pid,{getpid(),info->si_pid, identifier}});
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