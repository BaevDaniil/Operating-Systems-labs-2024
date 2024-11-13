#include "host.hpp"

template <Conn T>
Host<T>::Host(const std::string &path) : pid(getpid()), table()
{
    std::ofstream file(path);
    if(!file)
        throw std::runtime_error("No config file");
    file << pid;
    file.close();
}