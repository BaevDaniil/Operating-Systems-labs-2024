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
#include <QMessageBox>
#include "../connections/connection.hpp"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr) : QMainWindow(parent)
    {
        QWidget *centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);

        QWidget *ChatWidget = new QWidget(this);
        QVBoxLayout *ChatLayout = new QVBoxLayout(ChatWidget);

        QLabel *ChatLabel = new QLabel("Chat", this);
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

        QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
        mainLayout->addWidget(ChatWidget);
        setWindowTitle((name + "Chat Application ").c_str());
        resize(800, 600);
        connect(SendButton, &QPushButton::clicked, this, [this]()
        {
            std::string msg = InputField->text().toStdString();
            set_msg_to_chat("my: " + msg);
            send_msg_to_chat(msg); 
        });
    }
    MainWindow(int pid_, QWidget *parent = nullptr) : MainWindow(parent)
    {
        pid = pid_;
    }

    MainWindow(const std::string &name_, bool indent, QWidget *parent = nullptr)  : MainWindow(parent)
    {
        name = name_;
        is_host = indent;
        setWindowTitle((name + "Chat Application ").c_str());
    }
    ~MainWindow() = default;

private:
    std::string name;
    bool is_host;
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

    void send_msg_to_chat(const std::string &msg);

    void no_client_notification()
    {
        QMessageBox::critical(this, "!", "Client is dead");
    }
};