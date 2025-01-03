#pragma once
#include "connection.hpp"

class ShmConnection final : public Connection {
private:
    std::string name;
    int fd;   // Файловый дескриптор для shm
    void *addr;  // Адрес памяти для чтения/записи
    static constexpr size_t max_msg_size = 1024;

public:
    ShmConnection(const std::string &id, bool create);
    virtual bool Read(std::string &) override;
    virtual bool Write(const std::string &) override;
    static std::string to_string() { return "shm"; }
    virtual ~ShmConnection() override;

    static std::string create_filename(int num1, int num2) {
        return "/" + to_string() + "_" + std::to_string(num1) + '_' + std::to_string(num2);
    }
    static std::string create_general_filename(int num1, int num2) {
        return create_filename(num1, num2) + "_general";
    }

    void print_name() override{
        std::cout << "Conn name " << name;
    }
};
