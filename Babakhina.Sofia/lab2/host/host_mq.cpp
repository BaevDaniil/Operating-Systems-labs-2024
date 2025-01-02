#include "../connection/conn_mq.h"
#include "../host/host_mq.h"


bool HostMq::setup_conn() {
    pid_t pid = getpid();
    std::ofstream pid_file("host_pid.txt");
    if (!pid_file) {
        std::cout << "Failed to open PID file for writing";
        return false;
    }
    pid_file << pid;
    pid_file.close();

    std::string client_path = "/client_mq" + std::to_string(pid);
    std::string host_path = "/host_mq" + std::to_string(pid);

    key_t key_client = ftok(client_path.c_str(), 1);
    key_t key_host = ftok(host_path.c_str(), 1);

    client_conn = new ConnMq(key_client, true);
    if (!client_conn->is_valid()) {
        std::cout << "Failed to open MQ for writing\n";
        return false;
    }

    host_conn = new ConnMq(key_host, true);
    if (!host_conn->is_valid()) {
        std::cout << "Failed to open MQ for reading\n";
        return false;
    }
    return true;
}


HostMq::HostMq(const std::vector<Book>& books_) : Host(std::move(books_)) {
    setup_conn();
}


HostMq::~HostMq() {
    delete client_conn;
    delete host_conn;
}


int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    HostMq host(books);
    std::thread read_thread(read_wrap, std::ref(host));
    host.window.show();
    int res = app.exec();
    host.is_running = false;
    if (read_thread.joinable()) {
        read_thread.join();
    }
    
    return res;
}