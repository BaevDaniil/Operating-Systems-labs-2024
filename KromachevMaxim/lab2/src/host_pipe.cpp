#include "host_window.h"
#include "client_window.h"
#include "includes.h"
#include "conn_pipe.h"

void HostChatWindow::setup_conn()
{
    for(size_t i = 0; i < clients_pid.size(); i++)
    {
        publics_conn[clients_pid[i]].first = std::make_unique<ConnPipe>(*((int*)reserve + 2 * i));
        publics_conn[clients_pid[i]].second = std::make_unique<ConnPipe>(*((int*)reserve + 2 * i + 1));

        privates_conn[clients_pid[i]].first = std::make_unique<ConnPipe>(*((int*)reserve + 2 * i + 2));
        privates_conn[clients_pid[i]].second = std::make_unique<ConnPipe>(*((int*)reserve + 2 * i + 3));
    }
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
        exit(1);
    }

    int clientCount = std::stoi(argv[1]);
    if (clientCount <= 0)
    {
        fprintf(stderr, "Ошибка: некорректное количество клиентов.\n");
        exit(1);
    }

    std::vector<__pid_t> pid_clients;
    int pipes[1024];

    for(int i = 0; i < clientCount; i++)
    {
        // public connection
        int p1[2], p2[2];
        // private connection
        int p3[2], p4[2];

        if(pipe(p1) || pipe(p2) || pipe(p3) || pipe(p4))
        {
            std::perror("ошибка создания канала");
            exit(1);
        }
        pid_t pid = fork();

        if (pid == -1)
        {
            fprintf(stderr, "Ошибка: функция fork() вернула -1.\n");
            exit(1);

        }
        else if (pid == 0)
        {
            int argc = 0;
            char** argv = nullptr;

            close(p1[1]);
            close(p2[0]);
            close(p3[1]);
            close(p4[0]);
            int p[4] = {p2[1], p1[0], p4[1], p3[0]};

            QApplication app(argc, argv);
            ClientChatWindow client_window(getppid(), p);
            client_window.show();

            return app.exec();
        }
        else
        {
            pid_clients.push_back(pid);
            close(p1[0]);
            close(p2[1]);
            close(p3[0]);
            close(p4[1]);
            pipes[2 * i] = p1[1];
            pipes[2 * i + 1] = p2[0];
            pipes[2 * i + 2] = p3[1];
            pipes[2 * i + 3] = p4[0];
        }
    }

    QApplication app(argc, argv);
    HostChatWindow host_window(pid_clients, pipes);
    host_window.show();
    return app.exec();
}
