#include "client_seg.h"


bool ClientSeg::setup_conn() {
    client_conn = new ConnSeg(1, false);
    if (!client_conn->is_valid()) {
        std::cout << "Failed to open SEG for writing\n";
        return false;
    }

    host_conn = new ConnSeg(500, false);
    if (!host_conn->is_valid()) {
        std::cout << "Failed to open SEG for reading\n";
        return false;
    }
    return true;
};


ClientSeg::ClientSeg(std::vector<Book> books) : Client(std::move(books)) {
    setup_conn();
}


ClientSeg::~ClientSeg() {
    delete client_conn;
    delete host_conn;
    delete semaphore;
}

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    ClientSeg client(books);
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
