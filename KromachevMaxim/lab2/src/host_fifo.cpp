#include "host_window.h"
#include "client_window.h"
#include "includes.h"
#include "conn_fifo.h"

void HostChatWindow::setup_conn()
{
    std::string send_public_chat = "/tmp/host_client_public_fifo" + std::to_string(getpid());
    std::string read_public_chat = "/tmp/client_host_public_fifo" + std::to_string(getpid());

    public_conn.first = new ConnFifo(send_public_chat, true);
    public_conn.second = new ConnFifo(read_public_chat, true);

    for(auto& client_pid : clients_pid)
    {
        std::string send_private_chat = "/tmp/host_client_private_fifo" + std::to_string(client_pid);
        std::string read_private_chat = "/tmp/client_host_private_fifo" + std::to_string(client_pid);

        privates_conn[client_pid].first = new ConnFifo(send_private_chat, true);
        privates_conn[client_pid].second = new ConnFifo(read_private_chat, true);
    }

    QTimer *timer = new QTimer(this);

    connect(timer, &QTimer::timeout, [this, timer]()
    {
        if (setup_count < 10)
        {
            if (!public_conn.first->is_valid())
            {
                public_conn.first->setup_conn(true);
            }

            if (!public_conn.second->is_valid())
            {
                public_conn.second->setup_conn(true);
            }

            for(auto& conn : privates_conn)
            {
                if (!conn.second.first->is_valid())
                {
                    conn.second.first->setup_conn(true);
                }

                if (!conn.second.second->is_valid())
                {
                    conn.second.second->setup_conn(true);
                }
            }

            bool is_valid = true;
            for(auto& conn : privates_conn)
            {
                if(!conn.second.first->is_valid() || !conn.second.second->is_valid())
                {
                    is_valid = false;
                    break;
                }
            }

            if (public_conn.first->is_valid() && public_conn.second->is_valid() && is_valid) {
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

void HostChatWindow::send_public_msg(const std::string& msg)
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

void HostChatWindow::send_private_msg(const std::string& msg)
{
    InputDialog *chat_dialog = qobject_cast<InputDialog*>(sender());

    for(auto& private_chat : private_chats)
    {
        if(private_chat.second.get() == chat_dialog)
        {
            __pid_t pid;
            const char* str = private_chat.first->text().toStdString().c_str();
            sscanf(str, "Клиент: %d", &pid);

            auto private_conn = privates_conn[pid];

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

            private_chat.second->append_msg(msg);
            return;
        }
    }
}

void HostChatWindow::read_msg()
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

    for(auto& private_conn : privates_conn)
    {
        if(private_conn.second.second->is_valid())
        {
            std::string msg;
            const unsigned max_size = 1024;
            if(private_conn.second.second->read(msg, max_size))
            {
                if(!msg.empty())
                {
                    for(auto& private_chat : private_chats)
                    {
                        __pid_t pid;
                        const char* str = private_chat.first->text().toStdString().c_str();
                        sscanf(str, "Клиент: %d", &pid);

                        if(pid == private_conn.first)
                        {
                            private_chat.second->append_msg(">>> " + msg);
                            break;
                        }
                    }
                }
            }
        }
    }
}

int main(int argc, char *argv[])
{

    if (argc < 2)
    {
        fprintf(stderr, "Ошибка: укажите количество клиентов в аргументах командной строки.\n");
        return 1;
    }

    int clientCount = std::stoi(argv[1]);
    if (clientCount <= 0)
    {
        fprintf(stderr, "Ошибка: некорректное количество клиентов.\n");
        return 1;
    }

    std::vector<__pid_t> pid_clients;

    for(int i = 0; i < clientCount; i++) {

        pid_t pid = fork();

        if (pid == -1)
        {
            fprintf(stderr, "Ошибка: функция fork() вернула -1.\n");
            return 1;

        }
        else if (pid == 0)
        {
            int argc = 0;
            char** argv = nullptr;

            QApplication app(argc, argv);
            ClientChatWindow client_window(getppid());
            client_window.show();

            return app.exec();
        }
        else
        {
            pid_clients.push_back(pid);
        }
    }

    QApplication app(argc, argv);
    HostChatWindow host_window(pid_clients);
    host_window.show();
    return app.exec();
}
