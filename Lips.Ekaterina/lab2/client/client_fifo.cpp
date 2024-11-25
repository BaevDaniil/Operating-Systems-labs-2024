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
    
    pid_file >> host_pid;
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
    while (is_running) {
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += conn_timeout;
        // if (sem_timedwait(semRead, &ts) == -1)
        //     return;

        if (client_conn && client_conn->is_valid()) {
            std::string message;
            const size_t max_size = 1024;

            struct timespec ts;
            clock_gettime(CLOCK_REALTIME, &ts);
            ts.tv_sec += conn_timeout;
            // if (sem_timedwait(semRead, &ts) == -1)
            //     return;

            if (client_conn->read(message, max_size)) {
                std::cout << "Read message: " << message << "\n";

                if (message == "YES") { 
                    std::cout << "Get YES\n";
                    window.success_take_book(); 
                }
                else if (message == "NO") { 
                    std::cout << "Get NO\n";
                    window.fail_take_book();
                }
            }
        }
        sleep(1);
    }
};


Client::Client(std::vector<Book> books) : window(books){
    setup_conn();

    std::string sem_read_path = "/sem2" + std::to_string(host_pid);
    sem_read = sem_open(sem_read_path.c_str(), 0);
    if (sem_read == SEM_FAILED) {
        std::cout << "Could not open semaphore --> Terminate\n";
        exit(EXIT_FAILURE);
    }

    std::string sem_write_path = "/sem1" + std::to_string(host_pid);
    sem_write = sem_open(sem_write_path.c_str(), 0);
    if (sem_write == SEM_FAILED) {
        std::cout << "Could not open semaphore --> Terminate\n";
        exit(EXIT_FAILURE);
    }
}

void read_wrap(Client& client){
    client.read_from_host();
}


int main(int argc, char* argv[]) {

    QApplication app(argc, argv);

    Client client(books);

    std::thread read_thread(read_wrap, std::ref(client));

    QObject::connect(&client.window, &ClientWindow::bookSelected, [&client](const QString& book_name) {
        std::string request = "TAKE " + book_name.toStdString();
        if (!client.host_conn->write(request)) {
            std::cout << "Failed to send request\n";
            return;
        }
        std::cout << "Write TAKE\n";
        sem_post(client.sem_write);
    });

    QObject::connect(&client.window, &ClientWindow::bookReturned, [&client] (const QString& book_name) {
        std::string request = "RETURN " + book_name.toStdString();
        if (!client.host_conn->write(request)) {
            std::cout << "Failed to send request\n";
            return;
        }
        std::cout << "Write RETURN\n";
        sem_post(client.sem_write);
    });

    client.window.setWindowTitle("Chat Application Client");
    client.window.resize(400, 300);
    client.window.show();

    int res = app.exec();

    client.is_running = false;

    if (read_thread.joinable()) {
        read_thread.join();
    }
    
    return res;
};
