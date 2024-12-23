#include "../interface/host_interface.h"
#include "../connection/conn_sock.h"
#include "../host/host.h"
#include "../book.h"


Host::Host(const std::vector<Book>& books_) : books(std::move(books_)), semaphore(host_pid, 1), window(books) {}


bool Host::create_pid_file() {
    std::ofstream pid_file("host_pid.txt");
    if (!pid_file) {
        std::cout << "Failed to open PID file for writing" << std::endl;
        return false;
    }
    pid_file << host_pid;
    pid_file.close();
    return true;
}


void Host::write_to_client(const std::string& response) {
    if (client_conn && client_conn->is_valid()) {
        if (!client_conn->write(response)) {
            std::cout << "Failed to write response \n";
            return;
        }
        std::cout << "Host write: " << response << "\n";
    }
}


void Host::read_from_client() {
    while (is_running) {

        if (host_conn && host_conn->is_valid()) {
            std::string message;
            const size_t max_size = 1024;

            semaphore.wait();
            
            if (host_conn->read(message, max_size)) {
                std::cout << "Success read: " << message << "\n";
                std::string state;
                std::string client_name;
                std::string book_name;
                std::string time;
                bool flag = true;
                if (message.rfind("TAKE ", 0) == 0) {
                    state = "[TAKE]";
                    std::cout << "Read TAKE: " << message << "\n";
                    std::string str = message.substr(5);
                    int delimetr_pos = str.find("#");
                    book_name = str.substr(0, delimetr_pos);
                    str = str.substr(delimetr_pos + 1);
                    delimetr_pos = str.find("#");
                    client_name = str.substr(0, delimetr_pos);
                    time = str.substr(delimetr_pos + 1);
                    std::string response;
                    if (take_book(book_name, client_name)){
                        response = "YES" + book_name + "#" + time;
                    }
                    else {
                        response = "NO" + book_name + "#" + time;
                        flag = false;
                    }
                    write_to_client(response);
                    std::cout << "Write: " << response << "\n";
                } 
                else if (message.rfind("RETURN ", 0) == 0) {
                    state = "[RETURN]";
                    std::cout << "Read RETURN: " << message << "\n";
                    std::string str = message.substr(7);
                    int delimetr_pos = str.find("#");
                    book_name = str.substr(0, delimetr_pos);
                    str = str.substr(delimetr_pos + 1);
                    delimetr_pos = str.find("#");
                    client_name = str.substr(0, delimetr_pos);
                    time = str.substr(delimetr_pos + 1);
                    return_book(book_name, client_name); 
                    
                }

                window.update_history(books, state, book_name, client_name, time, flag);
            }

            semaphore.post();
        }
        
        sleep(0.1);
    }
}


bool Host::return_book(const std::string& book_name, const std::string& client_name) {
    for (auto& book : books) {
        if (book.name == book_name) {
            book.count++;
            std::cout << "Book returned: " << book_name << ", Client_name: " << client_name << "\n";
            
            reading_books.erase(std::remove(reading_books.begin(), reading_books.end(), book_name));

            return true;
        }
    }
    std::cout << "No such book in library: " << book_name << "\n";
    return false;
}


bool Host::take_book(const std::string& book_name, const std::string& client_name) {
    if (!reading_books.empty() && std::find(reading_books.begin(), reading_books.end(), book_name) != reading_books.end()) {
        std::cout << "The book has already been taken" << std::endl;
    }
    else {
        for (auto& book : books) {
            if (book.name == book_name) {
                if (book.count > 0) {
                    book.count--;
                    std::cout << "Book taken: " << book_name << ", Client_name: " << client_name << "\n";

                    reading_books.push_back(book_name);

                    return true;
                } 
                else {
                    std::cout << "Book not available: " << book_name << "\n";
                    return false;
                }
            }
        }
    }
    std::cout << "No such book in library: " << book_name << "\n";
    return false;
}


void read_wrap(Host& host) {
    host.read_from_client();
}