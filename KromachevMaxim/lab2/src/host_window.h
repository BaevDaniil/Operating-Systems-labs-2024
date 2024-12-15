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
    ~HostChatWindow();

private slots:
    void send_public_msg(const std::string& msg, __pid_t pid = -1);
    void send_private_msg(const std::string& msg);
    void read_msg();

private:
    QTabWidget tab_widget;
    InputDialog public_chat;
    QWidget private_chats_widget;
    QListWidget clients_list;
    std::unordered_map<QListWidgetItem*, InputDialog*> private_chats;

    std::vector<__pid_t> clients_pid;
    std::unordered_map<__pid_t, QTimer*> timers;

    std::unordered_map<__pid_t, std::pair<std::unique_ptr<Conn>, std::unique_ptr<Conn>>> privates_conn;
    std::unordered_map<__pid_t, std::pair<std::unique_ptr<Conn>, std::unique_ptr<Conn>>> publics_conn;

    static HostChatWindow* instance;

    int setup_count = 0;

    void setup_conn();
    void init_gui();
    void init_timers();
    void init_signals();

    void handle_signal(int signo, siginfo_t* info, void* context);
};

#endif // HOSTCHATWINDOW_H
