#include "client_window.h"
#include "conn_fifo.h"

void ClientChatWindow::setup_conn()
{
    int host_pid;
    sscanf(host_path.c_str(), "/tmp/host_fifo%d", &host_pid);
    std::string client_path = "/tmp/client_fifo" + std::to_string(getpid());
    std::string general_chat_path = "/tmp/host_fifo" + std::to_string(host_pid);

    host_conn = new ConnFifo(client_path, false);
    if (!host_conn->is_valid() && setupCallCount >= 10)
    {
        QMessageBox::critical(this, "Ошибка", "Ошибка открытия личного fifo канала.");
        return;
    }

    general_chat_conn = new ConnFifo(general_chat_path, false);
    if (!general_chat_conn->is_valid() && setupCallCount >= 10)
    {
        QMessageBox::critical(this, "Ошибка", "Ошибка открытия общего fifo канала.");
        return;
    }
}

void ClientChatWindow::send_msg_to_general()
{
}

void ClientChatWindow::send_msg_to_private()
{
}

void ClientChatWindow::read_msg_from_general()
{
}

void ClientChatWindow::read_msg_from_private()
{
}

