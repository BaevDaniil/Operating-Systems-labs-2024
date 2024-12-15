#include "client_window.h"
#include "conn_socket.h"

void ClientChatWindow::setup_conn()
{
    {
        std::string send_private_chat = "/tmp/socket_private_chat" + std::to_string(getpid());
        std::unique_ptr<ConnSocket> client = std::make_unique<ConnSocket>(send_private_chat, false);
        client->accept_conn();
        private_conn.first = std::move(client);
    }

    {
        std::string send_public_chat = "/tmp/socket_public_chat" + std::to_string(getpid());
        std::unique_ptr<ConnSocket> client = std::make_unique<ConnSocket>(send_public_chat, false);
        client->accept_conn();
        private_conn.first = std::move(client);
    }
}

void ClientChatWindow::send_public_msg(const std::string& msg)
{
}

void ClientChatWindow::send_private_msg(const std::string& msg)
{
}

void ClientChatWindow::read_msg()
{
}
