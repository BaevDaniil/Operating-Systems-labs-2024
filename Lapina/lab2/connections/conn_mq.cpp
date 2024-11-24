#include "conn_mq.h"

std::unique_ptr<Conn> Conn::GetConn(pid_t hostPid, pid_t clientPid, Type type) {
    return std::make_unique<ConnMq>(hostPid, clientPid, type);
}

ConnMq::ConnMq(pid_t hostPid, pid_t clientPid, Type type) : hostPid(hostPid), clientPid(clientPid),type(type){
    path = "/mq" + std::to_string(hostPid)+std::to_string(clientPid);
}

bool ConnMq::Open() {
    if (type == Type::HOST) {
        struct mq_attr attr;
        attr.mq_flags = 0;
        attr.mq_maxmsg = 1;
        attr.mq_msgsize = msgSize;
        attr.mq_curmsgs = 0;

        mqd = mq_open(path.c_str(), O_CREAT | O_RDWR, 0777, &attr);
    }
    else
        mqd = mq_open(path.c_str(), O_RDWR);
    return mqd != -1;

}

bool ConnMq::Read(void* buf, size_t count) {
    return mq_receive(mqd, (char*)buf, msgSize , nullptr) != -1;
}

bool ConnMq::Write(void* buf, size_t count) {
    return mq_send(mqd, (const char*)buf, count, 0) != -1;
}

void ConnMq::Close() {
    mq_close(mqd);
}