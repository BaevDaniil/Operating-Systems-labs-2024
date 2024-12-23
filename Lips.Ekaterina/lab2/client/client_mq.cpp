#include "client_mq.h"
#include "../book.h"
#include "../connection/conn_mq.h"


bool ClientMq::setup_conn() {
    std::string client_path = "/client_mq" + std::to_string(host_pid);
    std::string host_path = "/host_mq" + std::to_string(host_pid);

    key_t client_key = ftok(client_path.c_str(), 1);
    key_t host_key = ftok(host_path.c_str(), 1);

    client_conn = new ConnMq(client_key, false);
    if (!client_conn->is_valid()) {
        std::cout << "Failed to open MQ for writing\n";
        return false;
    }

    host_conn = new ConnMq(host_key, false);
    if (!host_conn->is_valid()) {
        std::cout << "Failed to open MQ for reading\n";
        return false;
    }
    return true;
}


ClientMq::ClientMq(const std::vector<Book>& books) : Client(std::move(books)) {}


ClientMq::~ClientMq() {
    delete client_conn;
    delete host_conn;
    delete semaphore;
}


int main(int argc, char* argv[]) {

    QApplication app(argc, argv);

    ClientMq client(books);

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
