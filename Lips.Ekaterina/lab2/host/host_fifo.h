#include "../host/host.h"

#include <QMessageBox>
#include <QApplication>
#include <QTimer>
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <vector>


class HostFifo : public Host {
public:

    HostFifo(const std::vector<Book>& books_);
    ~HostFifo();

    bool setup_conn() override;

};

