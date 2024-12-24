#include "../interface/host_interface.h"
#include "../connection/conn_sock.h"
#include "../host/host_sock.h"
#include "../book.h"


bool HostSock::setup_conn() {
    std::cout << "setup_conn\n";

    if (!create_pid_file()) {
        return false;
    }

    if (!create_port_file()) {
        return false;
    }

    host_listening_conn = new ConnSock(port, true);
    if (!host_listening_conn->is_valid()) {
        std::cout << "Failed to open Sock for writing\n";
        return false;
    }

    return true;
}


bool HostSock::create_port_file() {
    std::ofstream port_file("host_port.txt");
    if (!port_file) {
        std::cout << "Failed to open port file for writing" << std::endl;
        return false;
    }
    port_file << port;
    port_file.close();
    return true;
}


HostSock::HostSock(const std::vector<Book>& books_) : Host(books_) {}


HostSock::~HostSock() {
    delete client_conn;
}


void HostSock::wait_conn() {
    while ((is_running) && ((host_conn = ConnSock::accept(*host_listening_conn)) == nullptr)) {
        sleep(0.1);
    }
    delete host_listening_conn; // delete listening socket 
    
    client_conn = host_conn;
}


void thread_func_wrap(HostSock& host) {
    host.wait_conn();
    host.read_from_client();
}


int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    HostSock host(books);

    if (!host.setup_conn()) {
        std::cout << "Failed to setup connection" << std::endl;
        return 0;
    }

    std::thread read_thread(thread_func_wrap, std::ref(host));

    host.window.show();    

    int res = app.exec();

    host.is_running = false;

    if (read_thread.joinable()) {
        read_thread.join();
    }

    return res;
}