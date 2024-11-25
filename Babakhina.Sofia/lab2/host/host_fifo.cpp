#include "../gui/host_gui.h"
#include "../connection/conn_fifo.h"
#include "../host/host.h"
#include "../book.h"
#include <QMessageBox>
#include <QApplication>
#include <QTimer>
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <vector>

bool Host::setup_conn() {
    std::cout << "setup_conn\n";
    pid_t pid = getpid();
    std::ofstream pid_file("build/host_pid.txt");
    if (!pid_file) {
        std::cout << "Failed to open PID file for writing";
        return false;
    }
    pid_file << pid;
    pid_file.close();

    std::string client_path = "/tmp/chat_client_fifo" + std::to_string(pid);
    std::string host_path = "/tmp/chat_host_fifo" + std::to_string(pid);

    client_conn = new ConnFifo(client_path, true);
    if (!client_conn->is_valid()) {
        return false;
    }

    host_conn = new ConnFifo(host_path, true);
    if (!host_conn->is_valid()) {
        return false;
    }
    return true;
};


bool takeBook(std::vector<Book>& books, const std::string& bookName) {
    std::cout << "takeBook\n";
    for (auto& book : books) {
        if (book.name == bookName) {
            if (book.count > 0) {
                std::cout << "takeBook if\n";
                book.count--;
                return true;
            } else {
                std::cout << "takeBook else\n";
                return false;
            }
        }
    }
    return false;
}

bool returnBook(std::vector<Book>& books, const std::string& bookName) {
    for (auto& book : books) {
        if (book.name == bookName) {
            book.count++;
            return true;
        }
    }
    return false;
}

void Host::write_to_client(std::string response) {
    if (client_conn && client_conn->is_valid()) {
        if (!client_conn->write(response)) {
            std::cout << "Failed write" << "\n";
            return;
        }
        std::cout << "Success write: " << response << "\n";
        sem_post(semWrite);
    }
};

void Host::read_from_client() {
    std::cout << "read_from_client\n";

    while (is_running) {
        if (host_conn && host_conn->is_valid()) {
            std::string msg;
            const size_t max_size = 1024;

            struct timespec ts;
            clock_gettime(CLOCK_REALTIME, &ts);
            ts.tv_sec += timeout;
            if (host_conn->read(msg, max_size)) {
                std::cout << "Success read: " << msg << "\n";
                if (msg.rfind("TAKE ", 0) == 0) {
                    std::cout << "Read TAKE: " << msg << "\n";
                    std::string bookName = msg.substr(5);
                    std::string response;
                    if (takeBook(books, bookName)){
                        response = "YES";
                    }
                    else {
                        response = "NO";
                    }
                    write_to_client(response);
                    std::cout << "Write: " << response << "\n";
                } 
                else if (msg.rfind("RETURN ", 0) == 0) {
                    std::cout << "Read RETURN: " << msg;
                    std::string bookName = msg.substr(7);
                    returnBook(books, bookName);
                }
                window.updateBooks(books);
            }
        }
        sleep(1);
    }
};


void Host::Terminate(int status) {
    exit(status);
}


Host::Host(const std::vector<Book>& books_) : window(books_) {
    books = books_;
    std::cout << "Before setup_conn\n";
    setup_conn();
    std::cout << "After setup_conn\n";

    struct sigaction sig{};
    memset(&sig, 0, sizeof(sig));
    sig.sa_sigaction = SignalHandler;
    sig.sa_flags = SA_SIGINFO;
    sigaction(SIGTERM, &sig, nullptr);
    sigaction(SIGUSR1, &sig, nullptr);

    std::string semReadPath = "/sem1" + std::to_string(pid);
    semRead = sem_open(semReadPath.c_str(), O_CREAT | O_EXCL, 0777, 0);
    if (semRead == SEM_FAILED) {
        Terminate(EXIT_FAILURE);
    }

    std::string semWritePath = "/sem2" + std::to_string(pid);
    semWrite = sem_open(semWritePath.c_str(), O_CREAT | O_EXCL, 0777, 0);
    if (semWrite == SEM_FAILED) {
        Terminate(EXIT_FAILURE);
    }
    std::cout << "End of constructor\n";
}

void read_wrap(Host& host){
    host.read_from_client();
}


int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    Host host(books);

    std::thread listenerThread(read_wrap, std::ref(host));

    host.window.show();

    int res = app.exec();

    host.is_running = false;

    if (listenerThread.joinable()) {
        listenerThread.join();
    }
    
    return res;
};