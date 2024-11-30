#ifndef CLIENTCHATWINDOW_H
#define CLIENTCHATWINDOW_H

#include "includes.h"
#include "input_dialog.h"

class ClientChatWindow : public QWidget
{
    Q_OBJECT
public:
    explicit ClientChatWindow(QWidget *parent = nullptr);
    ~ClientChatWindow() override = default;

private slots:
    void send_msg();
    void read_msg();

private:
    QTabWidget tabWidget;
    InputDialog generalChat;
    InputDialog privateChat;

    void setup_conn();
};

#endif // CLIENTCHATWINDOW_H
