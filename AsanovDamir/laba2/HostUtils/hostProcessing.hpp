#include <QApplication>
#include <thread>
#include <vector>
#include <string>
#include <atomic>
#include "HostWindow.hpp"
#include "conn_sock.hpp"
#include "semaphore.hpp"
#include "logger.hpp"
#include "Book.hpp"
#include <signal.h>

bool takeBook(std::vector<Book>& books, const std::string& bookName, LoggerHost& logger) {
    for (auto& book : books) {
        if (book.name == bookName) {
            if (book.count > 0) {
                book.count--;
                logger.log(Status::INFO, "Book taken: " + bookName);
                return true;
            } else {
                logger.log(Status::INFO, "Book not available: " + bookName);
                return false;
            }
        }
    }

    logger.log(Status::INFO, "No such Book in library: " + bookName);
    return false;
}

bool returnBook(std::vector<Book>& books, const std::string& bookName, LoggerHost& logger) {
    for (auto& book : books) {
        if (book.name == bookName) {
            book.count++;
            logger.log(Status::INFO, "Book returned: " + bookName);
            return true;
        }
    }

    logger.log(Status::INFO, "No such Book in library: " + bookName);
    return false;
}

void listenForClientMessages(ConnSock& hostSocket, Semaphore& semaphore, std::vector<Book>& books, HostWindow& window, std::atomic<bool>& is_running) {
    auto& logger = LoggerHost::get_instance();

    ConnSock* conn = hostSocket.Accept(logger);
    if (!conn)
    {
        logger.log(Status::ERROR, "Failed to accept connection");
        return;
    }

    while (is_running) {
        semaphore.Wait(); // Start critical section

        char buffer[1024] = {0};
        if (conn->Read(buffer, sizeof(buffer))) {
            std::string request(buffer);
            logger.log(Status::INFO, "Request is recieved: " + request);

            if (request.rfind("TAKE ", 0) == 0) {
                std::string bookName = request.substr(5);
                if (takeBook(books, bookName, logger))
                {
                    std::string response = "YES";
                    if (conn->Write(response.c_str(), response.size())) {
                        logger.log(Status::INFO, "Host response successfully");
                    }
                    else {
                        logger.log(Status::ERROR, "Failed to response");
                    }
                }
                else
                {
                    std::string response = "NO";
                    if (conn->Write(response.c_str(), response.size())) {
                        logger.log(Status::INFO, "Host response successfully");
                    }
                    else {
                        logger.log(Status::ERROR, "Failed to response");
                    }
                }
            } else if (request.rfind("RETURN ", 0) == 0) {
                std::string bookName = request.substr(7);
                returnBook(books, bookName, logger); // client unexpected any response on this request
            }

            window.updateBooks(books);
        }

        semaphore.Post(); // End critical section
        sleep(1);
    }

    delete conn;
    logger.log(Status::INFO, "listenForClientMessages is over");
}

int processHost(int port, Semaphore& semaphore, ConnSock& hostSocket, std::vector<Book> books, QApplication& app, pid_t pid) {
    LoggerHost::get_instance().log(Status::INFO, "Host is running");
    std::atomic<bool> is_running(true);
    HostWindow window(port, books);
    window.clientPid = pid;

    // Start host's listenning in new thread
    std::thread listenerThread(listenForClientMessages, std::ref(hostSocket), std::ref(semaphore), std::ref(books), std::ref(window), std::ref(is_running));

    window.show();
    int result = app.exec(); // Start window in this thread

    // Complete thread with listenning socket
    is_running = false;
    if (listenerThread.joinable()) {
        listenerThread.join();
    }

    return result;
}