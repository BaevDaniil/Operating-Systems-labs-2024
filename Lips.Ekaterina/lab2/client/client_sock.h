#pragma once

#include "client.h"

#include <QMessageBox>
#include <QApplication>
#include <QTimer>
#include <fstream>
#include <unistd.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <csignal>


class ClientSock : public Client {    
public:

    ClientSock(std::vector<Book> books);
    ~ClientSock();
    
    bool setup_conn() override;

};

