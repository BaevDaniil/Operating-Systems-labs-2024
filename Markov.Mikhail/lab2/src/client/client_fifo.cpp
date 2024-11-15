#include "client.hpp"

namespace
{
    using TempClient = Client<NamedChannel>;
    bool identifier = false;
    std::filesystem::path host_pid_path = std::filesystem::current_path() / "host/host.txt";
}
inline void client_signal_handler(int sig, siginfo_t *info, void *context)
{
    std::cout << "signal was handled" << std::endl;
    static std::string msg;
    static std::string msg_general;
    switch (sig)
    {
    case SIGUSR1:
        std::cout << info->si_pid << std::endl;
        TempClient::get_instance(host_pid_path, identifier).read_from_host(msg);
        std::cout << msg << std::endl;
        msg.clear();
        break;
    case SIGUSR2:
        std::cout << info->si_pid << std::endl;
        TempClient::get_instance(host_pid_path, identifier).read_from_host_general(msg_general);
        std::cout << msg_general << std::endl;
        msg_general.clear();
        break;
    default:
        std::cout << info->si_pid << std::endl;
        break;
    }
}

int main()
{
    std::cout << getpid() << std::endl;

    TempClient::get_instance(host_pid_path, identifier);
    while(true)
    {
    }
    return 0;
}