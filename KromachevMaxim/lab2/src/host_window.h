#ifndef HOSTCHATWINDOW_H
#define HOSTCHATWINDOW_H

#include "includes.h"
#include "input_dialog.h"
#include "conn.h"

class HostChatWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit HostChatWindow(const std::vector<std::string>& clients_path, QWidget *parent = nullptr);
    ~HostChatWindow() override = default;

private slots:
    void send_msg();
    void read_msg();

signals:
    void setup_conn_signal();

private:
    QTabWidget tabWidget;
    InputDialog generalChat;
    QWidget privateChatsWidget;
    QListWidget clientListWidget;
    std::unordered_map<QListWidgetItem*, std::unique_ptr<InputDialog>> dialogHash;
    std::vector<std::string> clients_path;

    Conn* client_conn;
    Conn* general_chat_conn;

    void setup_conn();
    void init_gui();
};

#endif // HOSTCHATWINDOW_H
