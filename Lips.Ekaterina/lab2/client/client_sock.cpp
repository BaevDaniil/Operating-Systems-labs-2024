#include "client_sock.h"
#include "../book.h"
#include "../connection/conn_sock.h"


bool ClientSock::setup_conn() {
    std::ifstream port_file("host_port.txt");
    if (!port_file) {
        std::cout << "Failed to open port file\n";
        return false;
    }

    int port;

    port_file >> port;
    port_file.close();

    if (std::remove("host_port.txt") != 0) {
        std::cout << "Error deleting port file\n";
        return false;
    }

    client_conn = new ConnSock(port, false);
    if (!client_conn->is_valid()) {
        std::cout << "Failed to create Sock\n";
        return false;
    }
    host_conn = client_conn;

    return true;
}


ClientSock::ClientSock(const std::vector<Book>& books) : Client(std::move(books)) {}


ClientSock::~ClientSock() {
    delete client_conn;
    delete semaphore;
}


int main(int argc, char* argv[]) {

    QApplication app(argc, argv);

    ClientSock client(books);

    if (!client.setup_conn()) {
        QMessageBox::critical(&client.window, "Error", "Failed to setup connection");
        std::cout << "Failed to setup connection" << std::endl;
        return 0;
    }

    std::thread read_thread(read_wrap, std::ref(client));

    client.write_to_host();

    client.window.show();

    int res = app.exec();

    client.is_running = false;
    client.client_conn->close();
    client.host_conn->close();

    if (read_thread.joinable()) {
        read_thread.join();
    }

    return res;
}
