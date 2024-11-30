#include "client_window.h"

ClientChatWindow::ClientChatWindow(QWidget *parent)
    : QWidget(parent)
{
    tabWidget.addTab(&generalChat, "Общий чат");
    tabWidget.addTab(&privateChat, "Личный чат");

    auto centralWidget = new QWidget(this);
    auto mainLayout = new QVBoxLayout(centralWidget);

    mainLayout->addWidget(&tabWidget);
    setLayout(mainLayout);

    setGeometry(100, 100, 800, 600);
    setWindowTitle("Клиент");
}

