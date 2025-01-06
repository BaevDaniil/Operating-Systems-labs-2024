#pragma once
#include "conn.hpp"

class MQConnection final : public Connection
{
    std::string name;
    mq_attr attr;
    const int max_msg_size = 1024;
    const int max_msg_count = 10;
    mqd_t mq;
public:
    MQConnection(const std::string &id, bool create);
    bool Read(std::string &) override;
    bool Write(const std::string &) override;
    static std::string to_string() { return "mq"; }
    static std::string make_filename(int pid1, int pid2)
    {
        return to_string() + '_' + std::to_string(pid1) + '_' + std::to_string(pid2);
    }
    ~MQConnection() override;
    MQConnection() = default;
    MQConnection(const MQConnection &) = default;
    MQConnection &operator=(const MQConnection &) = default;
    MQConnection(MQConnection &&) = default;
    MQConnection &operator=(MQConnection &&) = default;
};