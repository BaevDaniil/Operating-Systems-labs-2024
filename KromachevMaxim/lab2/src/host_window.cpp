#include "host_window.h"
#include "input_dialog.h"
#include "client_window.h"


HostChatWindow::HostChatWindow(const std::vector<__pid_t>& clients_pid, QWidget *parent)
    : QMainWindow(parent), clients_pid(clients_pid)
{
    init_gui();
    setup_conn();
    init_timers();
}

void HostChatWindow::init_gui()
{
    QScrollArea *scrollArea = new QScrollArea(&private_chats_widget);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    scrollArea->setWidget(&clients_list);

    QVBoxLayout *scrollLayout = new QVBoxLayout(&private_chats_widget);
    scrollLayout->addWidget(scrollArea);

    for(auto& client_pid : clients_pid)
    {
        auto itemText = QStringLiteral("Клиент: %1").arg(client_pid);
        auto *item = new QListWidgetItem(itemText, &clients_list);
        private_chats[item] = std::make_unique<InputDialog>();
        connect(private_chats[item].get(), &InputDialog::send_msg, this, &HostChatWindow::send_private_msg);
    }

    tab_widget.addTab(&public_chat, "Общий чат");
    tab_widget.addTab(&private_chats_widget, "Личный чат");

    auto centralWidget = new QWidget(this);
    auto mainLayout = new QVBoxLayout(centralWidget);

    mainLayout->addWidget(&tab_widget);
    setCentralWidget(centralWidget);

    setGeometry(100, 100, 800, 600);
    setWindowTitle("Сервер");

    connect(&public_chat, &InputDialog::send_msg, this, &HostChatWindow::send_public_msg);

    connect(&clients_list, &QListWidget::itemClicked, [this](QListWidgetItem *item)
    {
        if (private_chats.contains(item)) {
            private_chats[item]->show();
        }
    });
}

void HostChatWindow::init_timers()
{
    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &HostChatWindow::read_msg);
    timer->start(10);
}

