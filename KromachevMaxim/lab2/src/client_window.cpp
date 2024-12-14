#include "client_window.h"

ClientChatWindow::ClientChatWindow(__pid_t host_pid, QWidget *parent)
    : QWidget(parent), host_pid(host_pid)
{
    init_gui();
    setup_conn();
    init_timers();
}

void ClientChatWindow::init_gui()
{
    tab_widget.addTab(&public_chat, "Общий чат");
    tab_widget.addTab(&private_chat, "Личный чат");

    auto centralWidget = new QWidget(this);
    auto mainLayout = new QVBoxLayout(centralWidget);

    mainLayout->addWidget(&tab_widget);
    setLayout(mainLayout);

    setGeometry(100, 100, 800, 600);
    setWindowTitle(QString("Клиент: %1").arg(getpid()));

    connect(&public_chat, &InputDialog::send_msg, this, &ClientChatWindow::send_public_msg);
    connect(&private_chat, &InputDialog::send_msg, this, &ClientChatWindow::send_private_msg);
}

void ClientChatWindow::init_timers()
{
    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &ClientChatWindow::read_msg);
    timer->start(10);
}

