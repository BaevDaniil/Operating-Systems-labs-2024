#ifndef HOSTCHATWINDOW_H
#define HOSTCHATWINDOW_H

#include "includes.h"
#include "input_dialog.h"

class HostChatWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit HostChatWindow(QWidget *parent = nullptr);
    ~HostChatWindow() override = default;

private slots:
    void send_msg();
    void read_msg();

private:
    QTabWidget tabWidget;
    InputDialog generalChat;
    QWidget privateChatsWidget;
    QPushButton newClient;
    QListWidget clientListWidget;
    std::unordered_map<QListWidgetItem*, std::unique_ptr<InputDialog>> dialogHash;

    void setup_conn();
};

#endif // HOSTCHATWINDOW_H
