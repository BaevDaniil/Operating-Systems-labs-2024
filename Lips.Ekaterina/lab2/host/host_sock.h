#include "../host/host.h"

#include <QMessageBox>
#include <QApplication>
#include <QTimer>
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <vector>
#include <sys/ipc.h>


class HostSock : public Host {  
private:

    int port = 9095;

    ConnSock* host_listening_conn;

public:

    HostSock(const std::vector<Book>& books_);
    ~HostSock();

    bool setup_conn() override;
    bool create_port_file();
    void wait_conn();
    
};

void thread_func_wrap(HostSock& host);