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


class ClientMq : public Client {
public:

    ClientMq(const std::vector<Book>& books);
    ~ClientMq();
    
    bool setup_conn() override;

};

