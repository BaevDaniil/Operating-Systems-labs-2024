#ifndef CLIENTCHATWINDOW_H
#define CLIENTCHATWINDOW_H

#include "includes.h"
#include "input_dialog.h"
#include "conn.h"

class ClientChatWindow : public QWidget
{
    Q_OBJECT
public:
    explicit ClientChatWindow(__pid_t host_pid, void* reserve, QWidget *parent = nullptr);
    ~ClientChatWindow() override;

private slots:
    void send_public_msg(const std::string& msg);
    void send_private_msg(const std::string& msg);
    void read_msg();

private:
    QTabWidget tab_widget;
    InputDialog public_chat;
    InputDialog private_chat;
    __pid_t host_pid;

    int setup_count = 0;
    void* reserve;

    std::pair<std::unique_ptr<Conn>, std::unique_ptr<Conn>> private_conn {nullptr, nullptr};
    std::pair<std::unique_ptr<Conn>, std::unique_ptr<Conn>> public_conn {nullptr, nullptr};

    void setup_conn();
    void init_gui();
    void init_timers();
};

#endif // CLIENTCHATWINDOW_H
