#pragma once
#include "connection.hpp"

class MessageQueue final : public Connection
{
private:
    std::string name;
    const int max_msg_size = 1024;
    const int max_msg_count = 10;
    mqd_t mq;
public:
    MessageQueue(const std::string &id, bool create);
    virtual bool Read(std::string &) override;
    virtual bool Write(const std::string &) override;
    static std::string to_string() { return "mq"; }
    virtual ~MessageQueue() override;

    MessageQueue() = default;
    MessageQueue(const MessageQueue &) = default;
    MessageQueue &operator=(const MessageQueue &) = default;
    MessageQueue(MessageQueue &&) = default;
    MessageQueue &operator=(MessageQueue &&) = default;
};