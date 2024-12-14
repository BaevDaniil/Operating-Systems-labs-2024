#include "client_window.h"
#include "conn_fifo.h"

// TODO дописать деструкторы

void ClientChatWindow::setup_conn()
{
    std::string send_private_chat = "/tmp/client_host_private_fifo" + std::to_string(getpid());
    std::string read_private_chat = "/tmp/host_client_private_fifo" + std::to_string(getpid());

    private_conn.first = new ConnFifo(send_private_chat, false);
    private_conn.second = new ConnFifo(read_private_chat, false);

    std::string send_public_chat  = "/tmp/client_host_public_fifo"  + std::to_string(host_pid);
    std::string read_public_chat  = "/tmp/host_client_public_fifo"  + std::to_string(host_pid);

    public_conn.first = new ConnFifo(send_public_chat, false);
    public_conn.second = new ConnFifo(read_public_chat, false);

    QTimer *timer = new QTimer(this);

    connect(timer, &QTimer::timeout, [this, timer]()
    {
        if (setup_count < 10)
        {
            if (!public_conn.first->is_valid())
            {
                public_conn.first->setup_conn(false);
            }

            if (!public_conn.second->is_valid())
            {
                public_conn.second->setup_conn(false);
            }

            if (!private_conn.first->is_valid())
            {
                private_conn.first->setup_conn(false);
            }

            if (!private_conn.second->is_valid())
            {
                private_conn.second->setup_conn(false);
            }

            if (public_conn.first->is_valid() && public_conn.second->is_valid()
                    && private_conn.first->is_valid() && private_conn.second->is_valid()) {
                timer->stop();
                return;
            }

            ++setup_count;
        } else
        {
            timer->stop();
            QMessageBox::critical(this, "Ошибка", "Ошибка открытия общего fifo канала.");
        }
    });

    timer->start(1000);
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

    public_chat.append_msg(msg);
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
        if(private_conn.first->write(msg))
        {
            QMessageBox::critical(this, "Error", "Failed to send message.");
            return;
        }
    }

    private_chat.append_msg(msg);
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
                public_chat.append_msg(">>> " + msg);
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
                private_chat.append_msg(">>> " + msg);
            }
        }
    }
}


