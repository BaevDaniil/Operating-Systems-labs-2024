#include "conn_mq.hpp"

MessageQueue::MessageQueue(const std::string &id, bool create) : name(id)
{
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = max_msg_count;
    attr.mq_msgsize = max_msg_size;
    attr.mq_curmsgs = 0;

    mq = mq_open(name.c_str(), O_CREAT | O_RDWR, 0644, &attr);

    if (mq == (mqd_t)-1)
        throw std::runtime_error("Error opening message queue");
}

bool MessageQueue::Read(std::string& message)
{
    char buffer[max_msg_size];
    memset(buffer, 0, sizeof(buffer));

    ssize_t bytesReceived = mq_receive(mq, buffer, sizeof(buffer), nullptr);
    if (bytesReceived == -1)
        return false;

    message.assign(buffer, bytesReceived);
    return true;
}

bool MessageQueue::Write(const std::string& message)
{
    if (mq_send(mq, message.c_str(), message.size(), 0) == -1)
        return false;
    return true;
}

MessageQueue::~MessageQueue()
{
    mq_close(mq);
}