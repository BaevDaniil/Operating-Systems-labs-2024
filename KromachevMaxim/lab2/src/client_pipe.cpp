#include "client_window.h"
#include "conn_pipe.h"

void ClientChatWindow::setup_conn()
{
    public_conn.first = std::make_unique<ConnPipe>(*((int*)reserve));
    public_conn.second = std::make_unique<ConnPipe>(*((int*)reserve + 1));

    private_conn.first = std::make_unique<ConnPipe>(*((int*)reserve + 2));
    private_conn.second = std::make_unique<ConnPipe>(*((int*)reserve + 3));
}

void ClientChatWindow::send_public_msg(const std::string& msg)
{
    if(msg.empty())
    {
        QMessageBox::warning(this, "Warning", "Cannot send an empty message.");
        return;
    }

    if(public_conn.first && public_conn.first->is_valid())
    {
        if(!public_conn.first->write(msg))
        {
            QMessageBox::critical(this, "Error", "Failed to send message.");
            return;
        }
    }

    public_chat.append_msg("snd: " + msg);
}

void ClientChatWindow::send_private_msg(const std::string& msg)
{
    if(msg.empty())
    {
        QMessageBox::warning(this, "Warning", "Cannot send an empty message.");
        return;
    }

    if(private_conn.first && private_conn.first->is_valid())
    {
        if(!private_conn.first->write(msg))
        {
            QMessageBox::critical(this, "Error", "Failed to send message.");
            return;
        }
    }

    private_chat.append_msg("snd: " + msg);
}

void ClientChatWindow::read_msg()
{
    if(public_conn.second->is_valid())
    {
        std::string msg;
        const unsigned max_size = 1024;
        if(public_conn.second->read(msg, max_size))
        {
            if(!msg.empty())
            {
                public_chat.append_msg("rcv: " + msg);
            }
        }
    }

    if(private_conn.second->is_valid())
    {
        std::string msg;
        const unsigned max_size = 1024;
        if(private_conn.second->read(msg, max_size))
        {
            if(!msg.empty())
            {
                private_chat.append_msg("rcv: " + msg);
            }
        }
    }
}


