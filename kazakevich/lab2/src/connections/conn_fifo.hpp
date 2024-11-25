#pragma once

#include "connection.hpp"

class NamedChannel final : public Connection
{
private:
    std::string pathname;
    const int max_msg_size = 1024;
    int fd;
public:
    
    NamedChannel(const std::string &pathname, bool create);
    bool Read(std::string & message) override;
    bool Write(const std::string & message) override;
    static std::string to_string() { return "fifo"; }
    static std::string make_filename(int pid1, int pid2) 
    { 
        return to_string() + '_' + std::to_string(pid1) + '_' + std::to_string(pid2); 
    }
    static std::string make_general_filename(int pid1, int pid2) 
    { 
        return make_filename(pid1, pid2) + "_general"; 
    }
    NamedChannel() = default;
    NamedChannel(const NamedChannel&) = default;
    NamedChannel& operator = (const NamedChannel&) = default;
    NamedChannel(NamedChannel &&) = default;
    NamedChannel &operator=(NamedChannel &&) = default;
    virtual ~NamedChannel() override;
};