#include "client_window.h"

ClientChatWindow::ClientChatWindow(const std::string& host_path, QWidget *parent)
    : QWidget(parent), host_path(host_path)
{
    init_gui();
    setup_conn();

    auto timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this, timer]() {

        ++setupCallCount;
        setup_conn();

        if (setupCallCount >= 10) {
            timer->stop();
        }
    });
    timer->start(1000);
}

void ClientChatWindow::init_gui()
{
    tabWidget.addTab(&generalChat, "Общий чат");
    tabWidget.addTab(&privateChat, "Личный чат");

    auto centralWidget = new QWidget(this);
    auto mainLayout = new QVBoxLayout(centralWidget);

    mainLayout->addWidget(&tabWidget);
    setLayout(mainLayout);

    setGeometry(100, 100, 800, 600);
    setWindowTitle(QString("Клиент: %1").arg(getpid()));
}

