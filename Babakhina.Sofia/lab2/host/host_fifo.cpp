#include "../connection/conn_fifo.h"
#include "../host/host_fifo.h"


bool HostFifo::setup_conn() {
    pid_t pid = getpid();
    std::ofstream pid_file("host_pid.txt");
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
}


HostFifo::HostFifo(const std::vector<Book>& books_) : Host(std::move(books_)) {
    setup_conn();
}


HostFifo::~HostFifo() {
    delete client_conn;
    delete host_conn;
}


int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    HostFifo host(books);
    std::thread read_thread(read_wrap, std::ref(host));
    host.window.show();
    int res = app.exec();
    host.is_running = false;
    if (read_thread.joinable()) {
        read_thread.join();
    }

    return res;
}