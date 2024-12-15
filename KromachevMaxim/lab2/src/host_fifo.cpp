#include "host_window.h"
#include "client_window.h"
#include "includes.h"
#include "conn_fifo.h"

void HostChatWindow::setup_conn()
{
    for(auto& client_pid : clients_pid)
    {
        std::string send_private_chat = "/tmp/host_client_private_fifo" + std::to_string(client_pid);
        std::string read_private_chat = "/tmp/client_host_private_fifo" + std::to_string(client_pid);

        privates_conn[client_pid].first = std::make_unique<ConnFifo>(send_private_chat, true);
        privates_conn[client_pid].second = std::make_unique<ConnFifo>(read_private_chat, true);
    }

    for(auto& client_pid : clients_pid)
    {
        std::string send_public_chat = "/tmp/host_client_public_fifo" + std::to_string(client_pid);
        std::string read_public_chat = "/tmp/client_host_public_fifo" + std::to_string(client_pid);

        publics_conn[client_pid].first = std::make_unique<ConnFifo>(send_public_chat, true);
        publics_conn[client_pid].second = std::make_unique<ConnFifo>(read_public_chat, true);
    }

    QTimer *timer = new QTimer(this);

    connect(timer, &QTimer::timeout, [this, timer]()
    {
        if (setup_count < 10)
        {
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

            for(auto& conn : publics_conn)
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

            bool private_is_valid = true;
            for(auto& conn : privates_conn)
            {
                if(!conn.second.first->is_valid() || !conn.second.second->is_valid())
                {
                    private_is_valid = false;
                    break;
                }
            }

            bool public_is_valid = true;
            for(auto& conn : publics_conn)
            {
                if(!conn.second.first->is_valid() || !conn.second.second->is_valid())
                {
                    public_is_valid = false;
                    break;
                }
            }

            if (private_is_valid && public_is_valid) {
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

void HostChatWindow::send_public_msg(const std::string& msg, __pid_t pid)
{
    if(msg.empty())
    {
        QMessageBox::warning(this, "Warning", "Cannot send an empty message.");
        return;
    }

    for(auto& public_conn : publics_conn)
    {
        if(public_conn.second.first && public_conn.second.first->is_valid() && public_conn.first != pid)
        {
            if(!public_conn.second.first->write(msg))
            {
                QMessageBox::critical(this, "Error", "Failed to send message.");
                return;
            }
        }
    }

    if(pid == -1)
    {
        public_chat.append_msg("snd: " + msg);
        return;
    }
}

void HostChatWindow::send_private_msg(const std::string& msg)
{
    InputDialog *chat_dialog = qobject_cast<InputDialog*>(sender());

    for(auto& private_chat : private_chats)
    {
        if(private_chat.second == chat_dialog)
        {
            int pid;
            std::string  str = private_chat.first->text().toStdString();
            sscanf(str.c_str(), "Клиент: %d", &pid);

            if(msg.empty())
            {
                QMessageBox::warning(this, "Warning", "Cannot send an empty message.");
                return;
            }

            if(privates_conn[pid].first && privates_conn[pid].first->is_valid())
            {
                if(!privates_conn[pid].first->write(msg))
                {
                    QMessageBox::critical(this, "Error", "Failed to send message.");
                    return;
                }
            }

            private_chat.second->append_msg("snd: " + msg);
            return;
        }
    }
}

void HostChatWindow::read_msg()
{
    for(auto& public_conn : publics_conn)
    {
        if(public_conn.second.second->is_valid())
        {
            std::string msg;
            const unsigned max_size = 1024;
            if(public_conn.second.second->read(msg, max_size))
            {
                if(!msg.empty())
                {
                    send_public_msg(msg, public_conn.first);
                    public_chat.append_msg("rcv: " + msg);
                    timers[public_conn.first]->stop();
                    timers[public_conn.first]->start(60000);
                    break;
                }
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
                        int pid;
                        std::string  str = private_chat.first->text().toStdString();
                        sscanf(str.c_str(), "Клиент: %d", &pid);

                        if(pid == private_conn.first)
                        {
                            private_chat.second->append_msg("rcv: " + msg);
                            break;
                        }
                    }
                    timers[private_conn.first]->stop();
                    timers[private_conn.first]->start(60000);
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
