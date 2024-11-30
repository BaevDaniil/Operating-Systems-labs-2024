#include "host_window.h"
#include "input_dialog.h"


HostChatWindow::HostChatWindow(QWidget *parent)
    : QMainWindow(parent)
{
    newClient.setText("Добавить нового клиента(fifo)");

    QScrollArea *scrollArea = new QScrollArea(&privateChatsWidget);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    scrollArea->setWidget(&clientListWidget);

    QVBoxLayout *scrollLayout = new QVBoxLayout(&privateChatsWidget);
    scrollLayout->addWidget(scrollArea);
    scrollLayout->addWidget(&newClient);

    for (int i = 0; i < 20; ++i) {
        auto itemText = QStringLiteral("Элемент %1").arg(i + 1);
        auto *item = new QListWidgetItem(itemText, &clientListWidget);
        dialogHash[item] = std::make_unique<InputDialog>();
    }

    QObject::connect(&clientListWidget, &QListWidget::itemClicked, [this](QListWidgetItem *item) {
        if (dialogHash.contains(item)) {
            dialogHash[item]->show();
        }
    });

    tabWidget.addTab(&generalChat, "Общий чат");
    tabWidget.addTab(&privateChatsWidget, "Личный чат");

    auto centralWidget = new QWidget(this);
    auto mainLayout = new QVBoxLayout(centralWidget);

    mainLayout->addWidget(&tabWidget);
    setCentralWidget(centralWidget);

    setGeometry(100, 100, 800, 600);
    setWindowTitle("Сервер");
}

