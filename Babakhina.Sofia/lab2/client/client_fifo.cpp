#include "client.h"
#include "../book.h"
#include "../connection/conn_fifo.h"
#include <QMessageBox>
#include <QApplication>
#include <QTimer>
#include <fstream>
#include <unistd.h>
#include <iostream>
#include <thread>



bool Client::setup_conn() {
    std::ifstream pid_file("host_pid.txt");
    if (!pid_file) {
        std::cout << "Failed to open PID file\n";
        return false;
    }
    
    pid_t host_pid;
    pid_file >> host_pid;
    hostPid = host_pid;
    pid_file.close();

    if (std::remove("host_pid.txt") != 0) {
        std::cout << "Error deleting PID file\n";
        return false;
    }

    std::string client_path = "/tmp/chat_client_fifo" + std::to_string(host_pid);
    std::string host_path = "/tmp/chat_host_fifo" + std::to_string(host_pid);

    client_conn = new ConnFifo(client_path, false);
    if (!client_conn->is_valid()) {
        std::cout << "Failed to open FIFO for writing\n";
        return false;
    }

    host_conn = new ConnFifo(host_path, false);
    if (!host_conn->is_valid()) {
        std::cout << "Failed to open FIFO for reading\n";
        return false;
    }
    return true;
};

void Client::read_from_host() {
    std::cout << "read_from_host\n";
    while (is_running) {
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += connTimeout;
        if (sem_timedwait(semRead, &ts) == -1)
            return;

        if (client_conn && client_conn->is_valid()) {
            std::string msg;
            const size_t max_size = 1024;

            struct timespec ts;
            clock_gettime(CLOCK_REALTIME, &ts);
            ts.tv_sec += connTimeout;
            if (client_conn->read(msg, max_size)) {
                std::cout << "Read msg: " << msg << "\n";
                if (msg == "YES") { 
                    std::cout << "Get YES\n";
                    window.onSuccessTakeBook(); 
                }
                else if (msg == "NO") { 
                    std::cout << "Get NO\n";
                    window.onFailedTakeBook();
                }
            }
        }
        sleep(1);
    }
};

Client::Client(std::vector<Book> books) : window(books){
    std::cout << "Before setup_conn\n";
    setup_conn();
    std::cout << "After setup_conn\n";
    std::string semReadPath = "/sem2" + std::to_string(hostPid);
    semRead = sem_open(semReadPath.c_str(), 0);
    if (semRead == SEM_FAILED) {
        exit(EXIT_FAILURE);
    }

    std::string semWritePath = "/sem1" + std::to_string(hostPid);
    semWrite = sem_open(semWritePath.c_str(), 0);
    if (semWrite == SEM_FAILED) {
        exit(EXIT_FAILURE);
    }
    std::cout << "After create sem\n";
}

void read_wrap(Client& client){
    client.read_from_host();
}


int main(int argc, char* argv[]) {

    QApplication app(argc, argv);

    Client client(books);

    std::thread listenerThread(read_wrap, std::ref(client));

    QObject::connect(&client.window, &ClientWindow::bookSelected, [&client](const QString& bookName) {
        std::string request = "TAKE " + bookName.toStdString();
        if (!client.host_conn->write(request)) {
            return;
        }
        std::cout << "Write TAKE\n";
        sem_post(client.semWrite);
    });

    QObject::connect(&client.window, &ClientWindow::bookReturned, [&client] (const QString& bookName) {
        std::string request = "RETURN " + bookName.toStdString();
        if (!client.host_conn->write(request)) {
            return;
        }
        std::cout << "Write RETURN\n";
        sem_post(client.semWrite);
    });

    client.window.setWindowTitle("Chat Application Client");
    client.window.resize(400, 300);
    client.window.show();

    int res = app.exec();

    client.is_running = false;

    if (listenerThread.joinable()) {
        listenerThread.join();
    }
    
    return res;
};
