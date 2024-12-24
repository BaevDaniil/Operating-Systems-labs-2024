#include "../interface/host_interface.h"
#include "../connection/conn_fifo.h"
#include "../host/host_fifo.h"
#include "../book.h"


bool HostFifo::setup_conn() {
    std::cout << "setup_conn\n";

    if (!create_pid_file()) {
        return false;
    }

    std::ofstream pid_file("host_pid.txt");
    if (!pid_file) {
        std::cout << "Failed to open PID file for writing" << std::endl;
    }
    pid_file << host_pid;
    pid_file.close();

    std::string client_path = "/tmp/client_fifo" + std::to_string(host_pid);
    std::string host_path = "/tmp/host_fifo" + std::to_string(host_pid);

    client_conn = new ConnFifo(client_path, true);
    if (!client_conn->is_valid()) {
        std::cout << "Failed to open FIFO for writing" << std::endl;
        return false;
    }

    host_conn = new ConnFifo(host_path, true);
    if (!host_conn->is_valid()) {
        std::cout << "Failed to open FIFO for reading" << std::endl;
        return false;
    }
    return true;
}


HostFifo::HostFifo(const std::vector<Book>& books_) : Host(books_) {}


HostFifo::~HostFifo() {
    delete client_conn;
    delete host_conn;
}


int main(int argc, char* argv[]) {

    QApplication app(argc, argv);
    HostFifo host(books);

    if (!host.setup_conn()) {
        std::cout << "Failed to setup connection" << std::endl;
        return 0;
    }

    std::thread read_thread(read_wrap, std::ref(host));

    host.window.show();

    int res = app.exec();

    host.is_running = false;

    if (read_thread.joinable()) {
        read_thread.join();
    }
    
    return res;
}