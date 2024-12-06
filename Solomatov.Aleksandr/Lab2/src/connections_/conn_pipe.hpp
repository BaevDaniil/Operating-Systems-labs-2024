#pragma once
#include "connection.hpp"

class PipeConnection : public Connection {
private:
    std::string name;
    int fd; // file descriptor для pipe

public:
    PipeConnection(const std::string &id, bool create);
    virtual bool Read(std::string &) override;
    virtual bool Write(const std::string &) override;
    static std::string to_string() { return "pipe"; }
    virtual ~PipeConnection() override;

    PipeConnection() = default;
    PipeConnection(const PipeConnection &) = default;
    PipeConnection &operator=(const PipeConnection &) = default;
    PipeConnection(PipeConnection &&) = default;
    PipeConnection &operator=(PipeConnection &&) = default;

    static std::string create_filename(int num1, int num2) {
        return "/tmp/" + to_string() + "_" + std::to_string(num1) + '_' + std::to_string(num2);
    }

    static std::string create_general_filename(int num1, int num2) {
        return create_filename(num1, num2) + "_general";
    }
    void print_name() override{
        std::cout << "Conn name " << name;
    }
};
