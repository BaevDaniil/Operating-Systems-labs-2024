#include "client_window.h"
#include "conn_socket.h"

void ClientChatWindow::setup_conn()
{
    {
        std::string send_private_chat = "/tmp/socket_private_chat" + std::to_string(getpid());
        std::unique_ptr<ConnSocket> client = std::make_unique<ConnSocket>(send_private_chat, false);
        client->connect_to_server();
        private_conn.first = std::move(client);
    }

    {
        std::string send_public_chat = "/tmp/socket_public_chat" + std::to_string(getpid());
        std::unique_ptr<ConnSocket> client = std::make_unique<ConnSocket>(send_public_chat, false);
        client->connect_to_server();
        public_conn.first = std::move(client);
    }

    QTimer *timer = new QTimer(this);

    connect(timer, &QTimer::timeout, [this, timer]()
    {
        if (setup_count < 10)
        {
            if (!public_conn.first->is_valid())
            {
                ((ConnSocket*)public_conn.first.get())->connect_to_server();
            }

            if (!private_conn.first->is_valid())
            {
                ((ConnSocket*)private_conn.first.get())->connect_to_server();
            }

            if (public_conn.first->is_valid() && private_conn.first->is_valid() ) {
                timer->stop();
                return;
            }

            ++setup_count;
        } else
        {
            timer->stop();
            std::perror("Ошибка открытия socket");
            QMessageBox::critical(this, "Ошибка", "Ошибка открытия socket.");
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
    if(public_conn.first->is_valid())
    {
        std::string msg;
        const unsigned max_size = 1024;
        if(public_conn.first->read(msg, max_size))
        {
            if(!msg.empty())
            {
                public_chat.append_msg("rcv: " + msg);
            }
        }
    }

    if(private_conn.first->is_valid())
    {
        std::string msg;
        const unsigned max_size = 1024;
        if(private_conn.first->read(msg, max_size))
        {
            if(!msg.empty())
            {
                private_chat.append_msg("rcv: " + msg);
            }
        }
    }
}
