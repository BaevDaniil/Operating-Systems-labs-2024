#include "client_fifo.h"


bool ClientFifo::setup_conn() {
    std::string client_path = "/tmp/chat_client_fifo" + std::to_string(host_pid);
    std::string host_path = "/tmp/chat_host_fifo" + std::to_string(host_pid);

    client_conn = new ConnFifo(client_path, false);
    if (!client_conn->is_valid()) {
        std::cout << "Failed to open FIFO for writing\n";
        return false;
    }

    host_conn = new ConnFifo(host_path, false);
    if (!host_conn->is_valid()) {
        std::cout << "Failed to open FIFO for reading\n";
        return false;
    }
    return true;
};


ClientFifo::ClientFifo(std::vector<Book> books) : Client(std::move(books)) {
    setup_conn();
}


ClientFifo::~ClientFifo() {
    delete client_conn;
    delete host_conn;
    delete semaphore;
}

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    ClientFifo client(books);
    std::thread read_thread(read_wrap, std::ref(client));
    client.write_to_host();
    client.window.show();
    int res = app.exec();
    client.is_running = false;
    if (read_thread.joinable()) {
        read_thread.join();
    }
    
    return res;
};
