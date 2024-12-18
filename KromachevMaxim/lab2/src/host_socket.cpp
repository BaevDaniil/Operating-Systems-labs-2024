#include "host_window.h"
#include "client_window.h"
#include "includes.h"
#include "conn_socket.h"

void HostChatWindow::setup_conn()
{
    for(auto& client_pid : clients_pid)
    {
        std::string send_private_chat = "/tmp/socket_private_chat" + std::to_string(client_pid);
        std::unique_ptr<ConnSocket> server = std::make_unique<ConnSocket>(send_private_chat, true);
        server->accept_conn();
        privates_conn[client_pid].first = std::move(server);
    }

    for(auto& client_pid : clients_pid)
    {
        std::string send_public_chat = "/tmp/socket_public_chat" + std::to_string(client_pid);
        std::unique_ptr<ConnSocket> server = std::make_unique<ConnSocket>(send_public_chat, true);
        server->accept_conn();
        publics_conn[client_pid].first = std::move(server);
    }

    QTimer *timer = new QTimer(this);

    connect(timer, &QTimer::timeout, [this, timer]()
    {
        if (setup_count < 10)
        {
            for(auto& conn : privates_conn)
            {
                if (!conn.second.first->is_valid()) {
                    ((ConnSocket*)conn.second.first.get())->accept_conn();
                }
            }

            for(auto& conn : publics_conn)
            {
                if (!conn.second.first->is_valid()) {
                    ((ConnSocket*)conn.second.first.get())->accept_conn();
                }
            }

            bool private_is_valid = true;
            for(auto& conn : privates_conn)
            {
                if(!conn.second.first->is_valid()) {
                    private_is_valid = false;
                    break;
                }
            }

            bool public_is_valid = true;
            for(auto& conn : publics_conn)
            {
                if(!conn.second.first->is_valid()) {
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
        if(public_conn.second.first->is_valid())
        {
            std::string msg;
            const unsigned max_size = 1024;
            if(public_conn.second.first->read(msg, max_size))
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
        if(private_conn.second.first->is_valid())
        {
            std::string msg;
            const unsigned max_size = 1024;
            if(private_conn.second.first->read(msg, max_size))
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
        std::perror("Ошибка: укажите количество клиентов в аргументах командной строки");
        exit(1);
    }

    int clientCount = std::stoi(argv[1]);
    if (clientCount <= 0)
    {
        std::perror("Ошибка: некорректное количество клиентов");
        exit(1);
    }

    std::vector<__pid_t> pid_clients;

    for(int i = 0; i < clientCount; i++) {

        pid_t pid = fork();

        if (pid == -1)
        {
            std::perror("Ошибка функции fork()");
            exit(1);

        }
        else if (pid == 0)
        {
            int argc = 0;
            char** argv = nullptr;

            QApplication app(argc, argv);
            ClientChatWindow client_window(getppid(), nullptr);
            client_window.show();

            return app.exec();
        }
        else
        {
            pid_clients.push_back(pid);
        }
    }

    QApplication app(argc, argv);
    HostChatWindow host_window(pid_clients, nullptr);
    host_window.show();
    return app.exec();
}
