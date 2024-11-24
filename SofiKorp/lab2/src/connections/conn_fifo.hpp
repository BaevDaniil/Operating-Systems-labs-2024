#pragma once

#include "conn.hpp"

class FIFOConnection final : public Connection
{
    std::string pathname;
    const int max_msg_size = 1024;
    int fd;

public:
    
    FIFOConnection(const std::string &pathname, bool create);
    bool Read(std::string & message) override;
    bool Write(const std::string & message) override;
    static std::string to_string() { return "fifo"; }
    static std::string make_filename(int pid1, int pid2) 
    { 
        return to_string() + '_' + std::to_string(pid1) + '_' + std::to_string(pid2); 
    }
    ~FIFOConnection() override;

    FIFOConnection() = default;
    FIFOConnection(const FIFOConnection&) = default;
    FIFOConnection& operator = (const FIFOConnection&) = default;
    FIFOConnection(FIFOConnection &&) = default;
    FIFOConnection &operator=(FIFOConnection &&) = default;
};