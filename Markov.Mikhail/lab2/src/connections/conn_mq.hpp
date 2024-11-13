#pragma once
#include "connection.hpp"

class MessageQueue final : public Connection
{
    MessageQueue(const std::string& id, bool create);
    virtual bool Read(std::string&) override;
    virtual bool Write(const std::string&) override;
    virtual ~MessageQueue() override;
private:
    std::string name;
    const int max_msg_size = 1024;
    const int max_msg_count = 10;
    mqd_t mq;
};