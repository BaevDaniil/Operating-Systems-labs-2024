#ifndef CLIENTCHATWINDOW_H
#define CLIENTCHATWINDOW_H

#include "includes.h"
#include "input_dialog.h"
#include "conn.h"

class ClientChatWindow : public QWidget
{
    Q_OBJECT
public:
    explicit ClientChatWindow(const std::string& host_path, QWidget *parent = nullptr);
    ~ClientChatWindow() override = default;

private slots:
    void send_msg_to_general();
    void send_msg_to_private();
    void read_msg_from_general();
    void read_msg_from_private();

private:
    QTabWidget tabWidget;
    InputDialog generalChat;
    InputDialog privateChat;
    std::string host_path;
    int setupCallCount = 0;

    Conn* host_conn;
    Conn* general_chat_conn;

    void setup_conn();
    void init_gui();
};

#endif // CLIENTCHATWINDOW_H
