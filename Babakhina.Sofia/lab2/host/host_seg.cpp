#include "../connection/conn_seg.h"
#include "../host/host_seg.h"


bool HostSeg::setup_conn() {
    std::cout << "setup_conn\n";
    pid_t pid = getpid();
    std::ofstream pid_file("host_pid.txt");
    if (!pid_file) {
        std::cout << "Failed to open PID file for writing";
        return false;
    }
    pid_file << pid;
    pid_file.close();

    client_conn = new ConnSeg(1, true);
    if (!client_conn->is_valid()) {
        std::cout << "Failed to open SEG for writing\n";
        return false;
    }

    host_conn = new ConnSeg(500, true);
    if (!host_conn->is_valid()) {
        std::cout << "Failed to open SEG for reading\n";
        return false;
    }
    return true;
};


HostSeg::HostSeg(const std::vector<Book>& books_) : Host(std::move(books_)) {
    setup_conn();
}


HostSeg::~HostSeg() {
    delete client_conn;
    delete host_conn;
}


int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    HostSeg host(books);
    std::thread read_thread(read_wrap, std::ref(host));
    host.window.show();
    int res = app.exec();
    host.is_running = false;
    if (read_thread.joinable()) {
        read_thread.join();
    }

    return res;
}