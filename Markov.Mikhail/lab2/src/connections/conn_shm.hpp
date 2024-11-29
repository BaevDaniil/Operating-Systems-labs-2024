#pragma once
#include "connection.hpp"

class GeneralFiles final : public Connection
{
private:
    std::string name;
    const int max_msg_size = 1024;
    int res;
    int fd;
    void* addr;
    
public:
    GeneralFiles(const std::string &id, bool create);
    virtual bool Read(std::string &) override;
    virtual bool Write(const std::string &) override;
    static std::string to_string() { return "shm"; }
    static std::string make_filename(int pid1, int pid2)
    {
        return to_string() + '_' + std::to_string(pid1) + '_' + std::to_string(pid2);
    }
    static std::string make_general_filename(int pid1, int pid2)
    {
        return make_filename(pid1, pid2) + "_general";
    }
    virtual ~GeneralFiles() override;

    GeneralFiles() = default;
    GeneralFiles(const GeneralFiles &) = default;
    GeneralFiles &operator=(const GeneralFiles &) = default;
    GeneralFiles(GeneralFiles &&) = default;
    GeneralFiles &operator=(GeneralFiles &&) = default;
};