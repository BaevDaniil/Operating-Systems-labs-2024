#include "client.hpp"

template<Conn T>
Client<T>::Client(const std::string& host_pid_path)
{
    std::ifstream file(host_pid_path);
    if (!file)
        throw std::runtime_error("No config file");
    file >> host_pid;
    file.close();
    pid = getpid();
}