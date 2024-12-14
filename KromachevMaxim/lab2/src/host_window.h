#ifndef HOSTCHATWINDOW_H
#define HOSTCHATWINDOW_H

#include "includes.h"
#include "input_dialog.h"
#include "conn.h"

class HostChatWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit HostChatWindow(const std::vector<__pid_t>& clients_pid, QWidget *parent = nullptr);
    ~HostChatWindow() override = default;

private slots:
    void send_public_msg(const std::string& msg);
    void send_private_msg(const std::string& msg);
    void read_msg();

private:
    QTabWidget tab_widget;
    InputDialog public_chat;
    QWidget private_chats_widget;
    QListWidget clients_list;
    std::unordered_map<QListWidgetItem*, std::unique_ptr<InputDialog>> private_chats;

    std::unordered_map<__pid_t, std::pair<Conn*, Conn*>> privates_conn;
    std::pair<Conn*, Conn*> public_conn {nullptr, nullptr};
    std::vector<__pid_t> clients_pid;

    int setup_count = 0;

    void setup_conn();
    void init_gui();
    void init_timers();
};

#endif // HOSTCHATWINDOW_H
