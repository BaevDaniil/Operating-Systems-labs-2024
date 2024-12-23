#include "client.h"


Client::Client(std::vector<Book> books) : client_books(std::move(books)), window(client_books, client_pid) {
    read_host_pid();
    semaphore = new Semaphore(host_pid, 0);
}

void Client::read_host_pid() {
    std::ifstream pid_file("host_pid.txt");
    if (!pid_file) {
        std::cout << "Failed to open PID file\n";
        return;
    }
    
    pid_file >> host_pid;
    pid_file.close();

    if (std::remove("host_pid.txt") != 0) {
        std::cout << "Error deleting PID file\n";
        return;
    }
}


void Client::read_from_host() {
    while (is_running) {

        if (client_conn && client_conn->is_valid()) {
            std::string message;
            const size_t max_size = 1024;
            
            semaphore->wait();

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
                    window.restart_timer_wrap();
                    take_flag = false;
                    str = message.substr(2);
                }

                
                int delimetr_pos = str.find("#");
                std::string book_name = str.substr(0, delimetr_pos);
                std::string time = str.substr(delimetr_pos + 1); 

                window.update_books(books, "[TAKE]", book_name, time, take_flag);
            }

            semaphore->post();
        }
        sleep(0.1);
    }
}


void Client::write_to_host() {
    QObject::connect(&window, &ClientWindow::book_selected_signal, [this](const QString& book_name) {
        auto time = std::chrono::system_clock::now();
        std::time_t time_c = std::chrono::system_clock::to_time_t(time);
        std::tm* ttime = std::localtime(&time_c);

        std::ostringstream oss;
        oss << std::put_time(ttime, "%H:%M");

        std::string request = "TAKE " + book_name.toStdString() + "#" + client_name + "#" + oss.str();

        semaphore->wait();

        if (!host_conn->write(request)) {
            std::cout << "Failed to send request\n";
            return;
        }

        window.stop_timer_wrap();

        semaphore->post();

        std::cout << "Write: " << request << std::endl;
    });

    QObject::connect(&window, &ClientWindow::book_returned_signal, [this] (const QString& book_name) {
        auto time = std::chrono::system_clock::now();
        std::time_t time_c = std::chrono::system_clock::to_time_t(time);
        std::tm* ttime = std::localtime(&time_c);

        std::ostringstream oss;
        oss << std::put_time(ttime, "%H:%M");
        std::string request = "RETURN " + book_name.toStdString() + "#" + client_name + "#" + oss.str();
        semaphore->wait();

        if (!host_conn->write(request)) {
            std::cout << "Failed to send request\n";
            return;
        }
        window.restart_timer_wrap();

        semaphore->post();
        window.update_books(books, "[RETURN]", book_name.toStdString(), oss.str(), true);
        std::cout << "Write RETURN\n";
    });
}


void read_wrap(Client& client) {
    client.read_from_host();
}
