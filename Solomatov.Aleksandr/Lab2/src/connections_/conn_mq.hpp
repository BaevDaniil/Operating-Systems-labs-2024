#pragma once

#include "connection.hpp"

class MQConnection final : public Connection
{
private:
    std::string name;
    struct mq_attr attr;
    const int max_msg_size = 1024;
    const int max_msg_count = 10;
    mqd_t mq;
public:
    MQConnection(const std::string &id, bool create);
    virtual bool Read(std::string &) override;
    virtual bool Write(const std::string &) override;
    static std::string to_string() { return "mq"; }
    virtual ~MQConnection() override;

    MQConnection() = default;
    MQConnection(const MQConnection &) = default;
    MQConnection &operator=(const MQConnection &) = default;
    MQConnection(MQConnection &&) = default;
    MQConnection &operator=(MQConnection &&) = default;

    static std::string create_filename(int num1, int num2)
    {
        return to_string() + std::to_string(num1) + '_' + std::to_string(num2);
    }

    static std::string create_general_filename(int num1, int num2)
    {
        return create_filename(num1, num2) + "general";
    }
};