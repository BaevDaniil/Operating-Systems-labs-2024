#include "../host/host.h"


Host::Host(const std::vector<Book>& books_) : books(std::move(books_)), semaphore(pid, 1), window(books) {}


void Host::write_to_client(std::string response) {
    if (client_conn && client_conn->is_valid()) {
        if (!client_conn->write(response)) {
            std::cout << "Failed to write response \n";
            return;
        }
    }
}

void set_parametrs(std::string &message, std::string &client_name, std::string &book_name, std::string &time, int pos){
    std::string str = message.substr(pos);
    int delimetr_pos = str.find("#");
    book_name = str.substr(0, delimetr_pos);
    str = str.substr(delimetr_pos + 1);
    delimetr_pos = str.find("#");
    client_name = str.substr(0, delimetr_pos);
    time = str.substr(delimetr_pos + 1);              
}

void Host::read_from_client() {
    while (is_running) {
        if (host_conn && host_conn->is_valid()) {
            std::string message;
            const size_t max_size = 1024;
            semaphore.wait();
            if (host_conn->read(message, max_size)) {
                std::string state;
                std::string client_name;
                std::string book_name;
                std::string time;
                bool flag = true;
                if (message.rfind("TAKE ", 0) == 0) {
                    state = "TAKE";
                    set_parametrs(message, client_name, book_name, time, 5);
                    std::string response;
                    if (select_book(books, book_name, client_name)){
                        response = "YES" + book_name + "#" + time;
                    }
                    else {
                        response = "NO" + book_name + "#" + time;
                        flag = false;
                    }
                    write_to_client(response);
                } 
                else if (message.rfind("RETURN ", 0) == 0) {
                    state = "RETURN";
                    set_parametrs(message, client_name, book_name, time, 7);
                    return_book(books, book_name, client_name); 
                }
                window.update_books(books, state, book_name, client_name, time, flag);
            }
            semaphore.post();
        }
        sleep(0.5);
    }
}

bool return_book(std::vector<Book>& books, const std::string& book_name, std::string client_name) {
    for (auto& book : books) {
        if (book.name == book_name) {
            book.count++;
            return true;
        }
    }
    std::cout << "No such book in library: " << book_name << "\n";
    return false;
}


bool select_book(std::vector<Book>& books, const std::string& book_name, std::string client_name) {
    for (auto& book : books) {
        if (book.name == book_name) {
            if (book.count > 0) {
                book.count--;
                return true;
            } else {
                std::cout << "Book is not available: " << book_name << "\n";
                return false;
            }
        }
    }
    std::cout << "No such book in library: " << book_name << "\n";
    return false;
}

void read_wrap(Host& host) {
    host.read_from_client();
}
