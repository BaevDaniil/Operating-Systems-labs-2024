#include "conn_mq.h"


ConnMQ::ConnMQ(key_t key, bool host_flag): is_host(host_flag) {
    int flags = (is_host ? IPC_CREAT : 0) | 0666;
    queue_id = msgget(key, flags);

    if (queue_id == -1) {
        std::cout << "Failed to open message queue\n";
    }
}

ConnMQ::~ConnMQ() {
    if (is_valid()) { msgctl(queue_id, IPC_RMID, nullptr); }
}

bool ConnMQ::read(std::string& msg, size_t max_size) {
    std::cout << "read msg: " << msg << "\n";

    if (!is_valid()) { 
        return false; 
    }

    struct msgbuf {
        long mtype;
        char mtext[1024];
    } message;

    std::cout << "before msgrcv message.mtext: " << message.mtext << "\n";
    ssize_t bytesRead = msgrcv(queue_id, &message, sizeof(message.mtext), !is_host + 1, 0); // !is_host: if host -> read client else read host
    if (bytesRead == -1) { 
        return false; 
    }
    std::cout << "after msgrcv message.mtext: " << message.mtext << "\n";
    std::cout << "after msgrcv bytesRead: " << bytesRead << "\n";
    
    msg = message.mtext;

    msg.resize(bytesRead);
    std::cout << "after msgrcv msg.resize: " << msg << "\n";

    return true;
}

bool ConnMQ::write(const std::string& msg) {
    std::cout << "write msg: " << msg << "\n";

    if (!is_valid()) {
        return false;
    }

    struct msgbuf {
        long mtype;
        char mtext[1024];
    } message;

    message.mtype = is_host + 1; // if host -> write as host (+ 1 because mtype > 0)

    std::cout << "before memcpy message.mtext: " << message.mtext << "\n";
    std::cout << "before memcpy msg.c_str(): " << msg.c_str() << "\n";
    std::cout << "before memcpy msg.size(): " << msg.size() << "\n";
    std::memcpy(message.mtext, msg.c_str(), msg.size());
    message.mtext[msg.size()] = '\0';
    std::cout << "after memcpy message.mtext: " << message.mtext << "\n";
    if (msgsnd(queue_id, &message, msg.size(), 0) == -1) {
        return false;
    }

    return true;
}

bool ConnMQ::is_valid() const {
    return (queue_id != -1);
}
