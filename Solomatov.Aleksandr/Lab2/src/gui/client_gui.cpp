#include "client_gui.hpp"
#include "../client_/client.hpp"

void ClientMainWindow::send_to_all(const std::string &msg) {
    if (auto client = client_instance.lock()) {
        client->send_to_host_general(msg);
    }
}

void ClientMainWindow::send_to_host(const std::string &msg) {
    if (auto client = client_instance.lock()) {
        client->send_to_host(msg);
    }
}
