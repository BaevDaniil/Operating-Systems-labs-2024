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

        if (client_conn && client_conn->is_valid()) {
            std::string message;
            const size_t max_size = 1024;

            if (client_conn->read(message, max_size)) {
                std::cout << "Read message: " << message << "\n";
                bool take_flag = true;
                std::string str;
                if (message.rfind("YES", 0) == 0) { 
                    std::cout << "Get YES\n";
                    window.success_take_book();
                    str = message.substr(3);

                    
                }
                else if (message.rfind("NO", 0) == 0) { 
                    std::cout << "Get NO\n";
                    window.fail_take_book();
                    take_flag = false;
                    str = message.substr(2);
                }

                
                int delimetr_pos = str.find("#");
                std::string book_name = str.substr(0, delimetr_pos);
                std::string time = str.substr(delimetr_pos + 1); 

                window.update_books(books, "[TAKE]", book_name, time, take_flag);
            }
        }
        sleep(1);
    }
    std::cout << "while ended" << std::endl;
};


Client::Client(std::vector<Book> books) : client_books(std::move(books)), window(client_books){
    setup_conn();
}

Client::~Client() {
    delete client_conn;
    delete host_conn;
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
    });

    QObject::connect(&client.window, &ClientWindow::bookReturned, [&client] (const QString& book_name) {
        std::string request = "RETURN " + book_name.toStdString();
        if (!client.host_conn->write(request)) {
            std::cout << "Failed to send request\n";
            return;
        }
        std::cout << "Write RETURN\n";
    });

    client.window.setWindowTitle("Chat Application Client");
    client.window.show();

    int res = app.exec();


    client.is_running = false;

    if (read_thread.joinable()) {
        read_thread.join();
    }
    
    return res;
};
