#include "client_gui.hpp"
#include <QtWidgets/QMessageBox>

ClientGUI::ClientGUI(QWidget *parent)
    : QMainWindow(parent)
    , clientSegConn(1, false)
    , clientMqConn(1, false)
    , clientFifoConn(1, false)
    , activityTimer(new QTimer(this)) {

    centralWidget = new QWidget(this);
    layout = new QVBoxLayout(centralWidget);

    chatDisplay = new QTextEdit(centralWidget);
    messageInput = new QLineEdit(centralWidget);
    sendButton = new QPushButton("Send", centralWidget);

    layout->addWidget(chatDisplay);
    layout->addWidget(messageInput);
    layout->addWidget(sendButton);

    setCentralWidget(centralWidget);

    connect(sendButton, &QPushButton::clicked, this, &ClientGUI::onSendButtonClicked);
    connect(activityTimer, &QTimer::timeout, this, &ClientGUI::checkActivity);

    activityTimer->start(60000); // Таймер на 1 минуту
}

ClientGUI::~ClientGUI() {
    delete layout;
    delete centralWidget;
}

void ClientGUI::onSendButtonClicked() {
    QString message = messageInput->text();
    if (!message.isEmpty()) {
        sendMessage(message);
        messageInput->clear();
        activityTimer->start(60000); // Перезапуск таймера
    }
}

void ClientGUI::sendMessage(const QString &message) {
    QByteArray messageData = message.toUtf8();
    clientSegConn.Write(messageData.constData(), messageData.size());
    clientMqConn.Write(messageData.constData(), messageData.size());
    clientFifoConn.Write(messageData.constData(), messageData.size());
}

void ClientGUI::receiveMessage() {
    char buffer[1024];
    clientSegConn.Read(buffer, sizeof(buffer));
    chatDisplay->append(QString::fromUtf8(buffer));

    clientMqConn.Read(buffer, sizeof(buffer));
    chatDisplay->append(QString::fromUtf8(buffer));

    clientFifoConn.Read(buffer, sizeof(buffer));
    chatDisplay->append(QString::fromUtf8(buffer));
}

void ClientGUI::checkActivity() {
    QMessageBox::warning(this, "Inactivity", "No messages sent in the last minute. Terminating client.");
    QApplication::quit();
}
