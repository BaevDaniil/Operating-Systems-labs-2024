#include "conn_message_queue.hpp"

MessageQueue::MessageQueue(const std::string &id, bool create)
{
    int flags = 0666 | (create ? IPC_CREAT : 0); 

    msgid = msgget(std::stoi(id), flags);
    if (msgid == -1)
        throw std::runtime_error("Failed to create or access message queue");
}

bool MessageQueue::Read(void *buf, size_t count)
{
    msgbuf *msg = (msgbuf *)malloc(sizeof(long) + count);
    if (!msg)
        throw std::runtime_error("Memory allocation failed");

    int result = msgrcv(msgid, msg, count, 0, 0); 
    if (result == -1)
    {
        free(msg);
        return false;
    }

    memcpy(buf, msg->mtext, count);
    free(msg);
    return true;
}

bool MessageQueue::Write(void *buf, size_t count)
{
    msgbuf *msg = (msgbuf *)malloc(sizeof(long) + count);
    if (!msg)
        throw std::runtime_error("Memory allocation failed");

    msg->mtype = 1; 
    memcpy(msg->mtext, buf, count);

    int result = msgsnd(msgid, msg, count, 0);
    free(msg);

    return result != -1;
}

MessageQueue::~MessageQueue()
{
    if (msgctl(msgid, IPC_RMID, nullptr) == -1)
        std::cerr << "Failed to remove message queue" << std::endl;
}