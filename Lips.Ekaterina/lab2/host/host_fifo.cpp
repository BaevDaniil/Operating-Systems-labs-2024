#include "../interface/host_interface.h"
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

bool pick_up_book(std::vector<Book>& books, const std::string& book_name) {
    std::cout << "takeBook\n";
    for (auto& book : books) {
        if (book.name == book_name) {
            if (book.count > 0) {
                book.count--;
                std::cout << "Book taken: " << book_name << "\n";
                return true;
            } else {
                std::cout << "Book not available: " << book_name << "\n";
                return false;
            }
        }
    }
    std::cout << "No such book in library: " << book_name << "\n";
    return false;
}


bool return_book(std::vector<Book>& books, const std::string& book_name) {
    for (auto& book : books) {
        if (book.name == book_name) {
            book.count++;
            std::cout << "Book returned: " << book_name << "\n";
            return true;
        }
    }
    std::cout << "No such book in library: " << book_name << "\n";
    return false;
}


bool Host::setup_conn() {
    std::cout << "setup_conn\n";
    pid_t pid = getpid();
    std::ofstream pid_file("build/host_pid.txt");
    if (!pid_file) {
        std::cout << "Failed to open PID file for writing\n";
        return false;
    }
    pid_file << pid;
    pid_file.close();

    std::string client_path = "/tmp/chat_client_fifo" + std::to_string(pid);
    std::string host_path = "/tmp/chat_host_fifo" + std::to_string(pid);

    client_conn = new ConnFifo(client_path, true);
    if (!client_conn->is_valid()) {
        std::cout << "Failed to open FIFO for writing\n";
        return false;
    }

    host_conn = new ConnFifo(host_path, true);
    if (!host_conn->is_valid()) {
        std::cout << "Failed to open FIFO for reading\n";
        return false;
    }
    return true;
};


void Host::write_to_client(std::string response) {
    if (client_conn && client_conn->is_valid()) {
        if (!client_conn->write(response)) {
            std::cout << "Failed to write response \n";
            return;
        }
        std::cout << "Success write: " << response << "\n";
        sem_post(sem_write);
    }
};


void Host::read_from_client() {
    while (is_running) {
        if (host_conn && host_conn->is_valid()) {
            std::string msg;
            const size_t max_size = 1024;

            struct timespec ts;
            clock_gettime(CLOCK_REALTIME, &ts);
            ts.tv_sec += timeout;
            // if (sem_timedwait(semRead, &ts) == -1) {
            //     std::cout << "over time\n";
            //     return;
            // }
            if (host_conn->read(msg, max_size)) {
                std::cout << "Success read: " << msg << "\n";
                if (msg.rfind("TAKE ", 0) == 0) {
                    std::cout << "Read TAKE: " << msg << "\n";
                    std::string bookName = msg.substr(5);
                    std::string response;
                    if (pick_up_book(books, bookName)){
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
                    return_book(books, bookName); 
                }

                window.update_books(books);
            }
        }
        sleep(1);
    }
};


Host::Host(const std::vector<Book>& books_) : window(books_) {
    books = books_;
    setup_conn();

    struct sigaction sig{};
    memset(&sig, 0, sizeof(sig));
    sig.sa_sigaction = signal_handler;
    sig.sa_flags = SA_SIGINFO;
    sigaction(SIGTERM, &sig, nullptr);
    sigaction(SIGUSR1, &sig, nullptr);

    std::string sem_read_path = "/sem1" + std::to_string(pid);
    sem_read = sem_open(sem_read_path.c_str(), O_CREAT | O_EXCL, 0777, 0);
    if (sem_read == SEM_FAILED) {
        terminate(EXIT_FAILURE);
    }

    std::string sem_write_path = "/sem2" + std::to_string(pid);
    sem_write = sem_open(sem_write_path.c_str(), O_CREAT | O_EXCL, 0777, 0);
    if (sem_write == SEM_FAILED) {
        terminate(EXIT_FAILURE);
    }
}


void read_wrap(Host& host){
    host.read_from_client();
}


int main(int argc, char* argv[]) {

    QApplication app(argc, argv);
    Host host(books);

    std::thread read_thread(read_wrap, std::ref(host));

    host.window.show();

    int res = app.exec();

    host.is_running = false;

    if (read_thread.joinable()) {
        read_thread.join();
    }
    
    return res;
};