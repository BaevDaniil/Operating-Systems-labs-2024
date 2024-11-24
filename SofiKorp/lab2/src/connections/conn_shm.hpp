#pragma once
#include "conn.hpp"

class SHMConnection final : public Connection
{
    std::string name;
    const int max_msg_size = 1024;
    int res;
    int fd;
    void* addr;
    
public:
    SHMConnection(const std::string &id, bool create);
    bool Read(std::string &) override;
    bool Write(const std::string &) override;
    static std::string to_string() { return "shm"; }
    static std::string make_filename(int pid1, int pid2)
    {
        return to_string() + '_' + std::to_string(pid1) + '_' + std::to_string(pid2);
    }
    ~SHMConnection() override;

    SHMConnection() = default;
    SHMConnection(const SHMConnection &) = default;
    SHMConnection &operator=(const SHMConnection &) = default;
    SHMConnection(SHMConnection &&) = default;
    SHMConnection &operator=(SHMConnection &&) = default;
};