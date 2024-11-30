#pragma once

#include <QApplication>
#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTextEdit>
#include <QLineEdit>
#include <QSplitter>
#include <QListWidget>
#include <QDialog>
#include <QLabel>
#include <QPalette>
#include <QScrollBar>
#include <QStyleFactory>
#include "../connections/connection.hpp"

class ClientMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    ClientMainWindow(QWidget *parent = nullptr) : QMainWindow(parent)
    {
        setWindowTitle("Client Chat Application");
        resize(1200, 900);

        setStyleSheet(
            "QMainWindow { background-color: #f5f5f5; }"
            "QTextEdit { background-color: #ffffff; border: 1px solid #cccccc; border-radius: 5px; }"
            "QLineEdit { background-color: #ffffff; border: 1px solid #cccccc; border-radius: 5px; padding: 5px; }"
            "QPushButton { background-color: #4CAF50; color: white; border-radius: 5px; padding: 8px 16px; }"
            "QPushButton:hover { background-color: #45a049; }"
            "QLabel { font-weight: bold; font-size: 14px; color: #333333; }");

        QWidget *centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);

        QSplitter *splitter = new QSplitter(this);

        QWidget *generalChatWidget = createChatWidget("General Chat", generalChatDisplay, generalInputField, generalSendButton);
        QWidget *privateChatWidget = createChatWidget("Private Chat with Host", ChatDisplay, InputField, SendButton);

        splitter->addWidget(generalChatWidget);
        splitter->addWidget(privateChatWidget);
        splitter->setStretchFactor(0, 1);
        splitter->setStretchFactor(1, 1);

        QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
        mainLayout->addWidget(splitter);

        connect(SendButton, &QPushButton::clicked, this, [this]()
                {
            std::string msg = InputField->text().toStdString();
            appendMessageToChat("Me: " + msg, ChatDisplay);
            sendMessageToChat(msg); });

        connect(generalSendButton, &QPushButton::clicked, this, [this]()
                {
            std::string msg = generalInputField->text().toStdString();
            appendMessageToChat("Me: " + msg, generalChatDisplay);
            sendMessageToGeneralChat(msg); });
    }

    ClientMainWindow(int client_pid, QWidget *parent = nullptr) : ClientMainWindow(parent)
    {
        pid = client_pid;
    }

    ~ClientMainWindow() = default;

private:
    QTextEdit *generalChatDisplay;
    QLineEdit *generalInputField;
    QPushButton *generalSendButton;
    QTextEdit *ChatDisplay;
    QLineEdit *InputField;
    QPushButton *SendButton;
    int pid;

    QWidget *createChatWidget(const QString &title, QTextEdit *&display, QLineEdit *&input, QPushButton *&sendButton)
    {
        QWidget *chatWidget = new QWidget(this);
        QVBoxLayout *chatLayout = new QVBoxLayout(chatWidget);

        QLabel *chatLabel = new QLabel(title, this);
        chatLabel->setAlignment(Qt::AlignCenter);
        chatLayout->addWidget(chatLabel);

        display = new QTextEdit(this);
        display->setReadOnly(true);
        display->verticalScrollBar()->setStyleSheet("QScrollBar { background: #f5f5f5; }");

        input = new QLineEdit(this);
        sendButton = new QPushButton("Send", this);

        QHBoxLayout *inputLayout = new QHBoxLayout();
        inputLayout->addWidget(input);
        inputLayout->addWidget(sendButton);

        chatLayout->addWidget(display);
        chatLayout->addLayout(inputLayout);

        return chatWidget;
    }

    void appendMessageToChat(const std::string &msg, QTextEdit *chatDisplay)
    {
        chatDisplay->append(QString::fromStdString(msg));
    }

public:
    void setMessageToChat(const std::string &msg)
    {
        appendMessageToChat(msg, ChatDisplay);
    }
    void setMessageToGeneralChat(const std::string &msg) 
    {
        appendMessageToChat(msg, generalChatDisplay);
    }
    void sendMessageToGeneralChat(const std::string &msg);
    void sendMessageToChat(const std::string &msg);
};
