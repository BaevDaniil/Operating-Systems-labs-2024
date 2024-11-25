#include "conn_mq.hpp"

MessageQueue::MessageQueue(const std::string &id, bool create) : name("/queue_" + id), attr()
{
    attr.mq_flags = 0;
    attr.mq_maxmsg = max_msg_count;
    attr.mq_msgsize = max_msg_size;
    attr.mq_curmsgs = 0;
    if (create)
        mq = mq_open(name.c_str(), O_CREAT | O_RDWR | O_NONBLOCK, 0777, &attr);
    else
        mq = mq_open(name.c_str(), O_RDWR | O_NONBLOCK);

    if (mq == (mqd_t)-1)
    {
        std::cerr << "Error opening message queue: " << strerror(errno) << std::endl;
        throw std::runtime_error("Error opening message queue");
    }
}

bool MessageQueue::Read(std::string& message)
{
    char buffer[max_msg_size];
    memset(buffer, 0, sizeof(buffer));

    mq = mq_open(name.c_str(), O_RDWR | O_NONBLOCK);

    if (mq == (mqd_t)-1)
    {
        std::cout << "Error opening message queue: " << strerror(errno) << std::endl;
        throw std::runtime_error("Error opening message queue");
    }

    ssize_t bytesReceived = mq_receive(mq, buffer, sizeof(buffer), nullptr);
    if (bytesReceived == -1)
        return false;

    message.assign(buffer, bytesReceived);
    return true;
}

bool MessageQueue::Write(const std::string& message)
{
    mq = mq_open(name.c_str(), O_RDWR | O_NONBLOCK);

    if (mq == (mqd_t)-1)
    {
        std::cerr << "Error opening message queue: " << strerror(errno) << std::endl;
        throw std::runtime_error("Error opening message queue");
    }

    if (mq_send(mq, message.c_str(), message.size(), 0) == -1)
        return false;
    return true;
}

MessageQueue::~MessageQueue()
{
    mq_close(mq);
}