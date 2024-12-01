#include "host_window.h"
#include "input_dialog.h"
#include "client_window.h"


HostChatWindow::HostChatWindow(const std::vector<std::string>& clients_path, QWidget *parent)
    : QMainWindow(parent), clients_path(clients_path)
{
    init_gui();
}


void HostChatWindow::init_gui()
{
    QScrollArea *scrollArea = new QScrollArea(&privateChatsWidget);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    scrollArea->setWidget(&clientListWidget);

    QVBoxLayout *scrollLayout = new QVBoxLayout(&privateChatsWidget);
    scrollLayout->addWidget(scrollArea);

    for(auto& client_path : clients_path)
    {
        int client_pid;
        sscanf(client_path.c_str(), "/tmp/client_fifo%d", &client_pid);
        auto itemText = QStringLiteral("Клиент: %1").arg(client_pid);
        auto *item = new QListWidgetItem(itemText, &clientListWidget);
        dialogHash[item] = std::make_unique<InputDialog>();
    }

    tabWidget.addTab(&generalChat, "Общий чат");
    tabWidget.addTab(&privateChatsWidget, "Личный чат");

    auto centralWidget = new QWidget(this);
    auto mainLayout = new QVBoxLayout(centralWidget);

    mainLayout->addWidget(&tabWidget);
    setCentralWidget(centralWidget);

    setGeometry(100, 100, 800, 600);
    setWindowTitle("Сервер");


    QObject::connect(&clientListWidget, &QListWidget::itemClicked, [this](QListWidgetItem *item) {
        if (dialogHash.contains(item)) {
            dialogHash[item]->show();
        }
    });
}

