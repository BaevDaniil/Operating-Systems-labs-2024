#include "conn_mq.h"


ConnMq::ConnMq(key_t key, bool host_flag): is_host(host_flag) {
    int flags = (is_host ? IPC_CREAT : 0) | 0666;
    queue_id = msgget(key, flags);

    if (queue_id == -1) {
        std::cout << "Failed to open message queue\n";
    }
}


ConnMq::~ConnMq() {
    if (is_valid()) { 
        close();
    }
}


bool ConnMq::read(std::string& msg, size_t max_size) {
    if (!is_valid()) return false;

    struct msgbuf {
        long mtype;
        char mtext[1024];
    };
    struct msgbuf message;

    ssize_t bytesRead = msgrcv(queue_id, &message, sizeof(message.mtext), !is_host + 1, IPC_NOWAIT);
    if (bytesRead == -1) {
        if (errno == ENOMSG) {
            // No message available, handle gracefully (e.g., return false or retry later)
            return false;
        } else {
            // Error other than "no message"
            // perror("msgrcv");
            return false;
        }
    }

    msg = message.mtext;
    msg.resize(bytesRead);
    return true;
}


bool ConnMq::write(const std::string& msg) {
    if (!is_valid()) return false;

    struct msgbuf {
        long mtype;
        char mtext[1024];
    };
    struct msgbuf message;

    message.mtype = is_host + 1;
    std::memcpy(message.mtext, msg.c_str(), std::min((size_t)sizeof(message.mtext) -1, msg.size())); // Protect against buffer overflow
    message.mtext[std::min((size_t)sizeof(message.mtext) -1, msg.size())] = '\0';

    int result = msgsnd(queue_id, &message, std::min((size_t)sizeof(message.mtext), msg.size()), IPC_NOWAIT);
    if (result == -1) {
        if (errno == ENOSPC) { //Message queue full
            return false; //Handle appropriately (e.g., retry, drop message)
        } else {
            perror("msgsnd");
            return false;
        }
    }
    return true;
}


bool ConnMq::is_valid() const {
    return (queue_id != -1);
}


void ConnMq::close() {
    msgctl(queue_id, IPC_RMID, nullptr); 
    queue_id = -1;
}
