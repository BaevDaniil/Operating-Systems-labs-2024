#pragma once

#include "client.h"

#include <QMessageBox>
#include <QApplication>
#include <QTimer>
#include <fstream>
#include <unistd.h>
#include <iostream>
#include <thread>


class ClientFifo : public Client { 
public:

    ClientFifo(const std::vector<Book>& books);
    ~ClientFifo();
    
    bool setup_conn() override;

};

