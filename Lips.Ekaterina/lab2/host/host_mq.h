#include "../host/host.h"

#include <QMessageBox>
#include <QApplication>
#include <QTimer>
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <vector>
#include <sys/ipc.h>


class HostMq : public Host {
public:

    HostMq(const std::vector<Book>& books_);
    ~HostMq();

    bool setup_conn() override;
    
};