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
#include "../includes/includes.hpp"

class ClientMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    ClientMainWindow(QWidget *parent = nullptr) : QMainWindow(parent)
    {
        QWidget *centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);

        QSplitter *splitter = new QSplitter(this);

        // Left side: General chat
        QWidget *generalChatWidget = new QWidget(this);
        QVBoxLayout *generalChatLayout = new QVBoxLayout(generalChatWidget);

        QLabel *generalChatLabel = new QLabel("General Chat", this);
        generalChatLabel->setAlignment(Qt::AlignCenter);
        generalChatLayout->addWidget(generalChatLabel);

        generalChatDisplay = new QTextEdit(this);
        generalChatDisplay->setReadOnly(true);

        generalInputField = new QLineEdit(this);
        generalSendButton = new QPushButton("Send", this);

        QHBoxLayout *generalInputLayout = new QHBoxLayout();
        generalInputLayout->addWidget(generalInputField);
        generalInputLayout->addWidget(generalSendButton);

        generalChatLayout->addWidget(generalChatDisplay);
        generalChatLayout->addLayout(generalInputLayout);

        // Right side: Client interfaces
        QWidget *ChatWidget = new QWidget(this);
        QVBoxLayout *ChatLayout = new QVBoxLayout(ChatWidget);

        QLabel *ChatLabel = new QLabel("Chat with host", this);
        ChatLabel->setAlignment(Qt::AlignCenter);
        ChatLayout->addWidget(ChatLabel);

        ChatDisplay = new QTextEdit(this);
        ChatDisplay->setReadOnly(true);

        InputField = new QLineEdit(this);
        SendButton = new QPushButton("Send", this);

        QHBoxLayout *InputLayout = new QHBoxLayout();
        InputLayout->addWidget(InputField);
        InputLayout->addWidget(SendButton);

        ChatLayout->addWidget(ChatDisplay);
        ChatLayout->addLayout(InputLayout);

        splitter->addWidget(generalChatWidget);
        splitter->addWidget(ChatWidget);

        QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
        mainLayout->addWidget(splitter);
        setWindowTitle("Client chat Application");
        resize(1200, 900);
        connect(SendButton, &QPushButton::clicked, this, [this]()
        {
            std::string msg = InputField->text().toStdString();
            set_msg_to_chat("my: " + msg);
            send_msg_to_chat(msg); 
        });
        connect(generalSendButton, &QPushButton::clicked, this, [this]()
        {
            std::string msg = generalInputField->text().toStdString();
            set_msg_to_general_chat("my: " + msg);
            send_msg_to_general_chat(msg); 
        });
    }
    ClientMainWindow(int client_pid, QWidget *parent = nullptr) : ClientMainWindow(parent)
    {
        pid = client_pid;
    }
    ~ClientMainWindow() = default;

public slots:
    void send() {}
    void read() {}

private:
    QTextEdit *generalChatDisplay;
    QLineEdit *generalInputField;
    QPushButton *generalSendButton;
    QTextEdit *ChatDisplay;
    QLineEdit *InputField;
    QPushButton *SendButton;
    int pid;

public:
    void set_msg_to_chat(const std::string &msg)
    {
        ChatDisplay->append(QString::fromStdString(msg));
    }

    void set_msg_to_general_chat(const std::string &msg)
    {
        generalChatDisplay->append(QString::fromStdString(msg));
    }

    void send_msg_to_general_chat(const std::string &msg);
    void send_msg_to_chat(const std::string &msg);
};