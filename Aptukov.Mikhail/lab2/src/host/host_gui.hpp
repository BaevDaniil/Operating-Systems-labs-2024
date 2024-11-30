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
#include <unordered_map>
#include <QString>
#include <iostream>
#include "../connections/connection.hpp"

class ChatWindow : public QDialog
{
    Q_OBJECT

public:
    ChatWindow(QWidget *parent = nullptr) : QDialog(parent)
    {
        setWindowTitle("Chat");
        setStyleSheet(
            "QDialog { background-color: #f9f9f9; }"
            "QTextEdit { background-color: #ffffff; border: 1px solid #cccccc; border-radius: 5px; }"
            "QLineEdit { background-color: #ffffff; border: 1px solid #cccccc; border-radius: 5px; padding: 5px; }"
            "QPushButton { background-color: #4CAF50; color: white; border-radius: 5px; padding: 8px 16px; }"
            "QPushButton:hover { background-color: #45a049; }"
            "QLabel { font-size: 14px; font-weight: bold; color: #333333; }");

        layout = new QVBoxLayout(this);

        chatDisplay = new QTextEdit(this);
        chatDisplay->setReadOnly(true);

        inputField = new QLineEdit(this);
        sendButton = new QPushButton("Send", this);

        inputLayout = new QHBoxLayout();
        inputLayout->addWidget(inputField);
        inputLayout->addWidget(sendButton);

        layout->addWidget(chatDisplay);
        layout->addLayout(inputLayout);

        resize(800, 600);
    }

    ChatWindow(int client_pid, QWidget *parent = nullptr) : ChatWindow(parent)
    {
        pid = client_pid;
        connect(sendButton, &QPushButton::clicked, this, [this]()
                {
            std::string msg = inputField->text().toStdString();
            send_msg(msg); });
    }

    ~ChatWindow() = default;

    void set_text(const std::string &msg)
    {
        chatDisplay->append(QString::fromStdString(msg));
    }

    void send_msg(const std::string &msg);

private:
    int pid;
    QVBoxLayout *layout;
    QTextEdit *chatDisplay;
    QPushButton *sendButton;
    QLineEdit *inputField;
    QHBoxLayout *inputLayout;

public:
    bool valid = true;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr) : QMainWindow(parent)
    {
        setWindowTitle("Host Chat Application");
        resize(1200, 900);
        setStyleSheet(
            "QMainWindow { background-color: #f5f5f5; }"
            "QTextEdit { background-color: #ffffff; border: 1px solid #cccccc; border-radius: 5px; }"
            "QLineEdit { background-color: #ffffff; border: 1px solid #cccccc; border-radius: 5px; padding: 5px; }"
            "QPushButton { background-color: #007BFF; color: white; border-radius: 5px; padding: 8px 16px; }"
            "QPushButton:hover { background-color: #0056b3; }"
            "QListWidget { border: 1px solid #cccccc; }"
            "QLabel { font-size: 14px; font-weight: bold; color: #333333; }");

        QWidget *centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);

        QSplitter *splitter = new QSplitter(this);

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

        QWidget *clientWidget = new QWidget(this);
        QVBoxLayout *clientLayout = new QVBoxLayout(clientWidget);

        QLabel *clientLabel = new QLabel("List of Clients", this);
        clientLabel->setAlignment(Qt::AlignCenter);
        clientLayout->addWidget(clientLabel);

        clientList = new QListWidget(this);
        clientLayout->addWidget(clientList);

        splitter->addWidget(generalChatWidget);
        splitter->addWidget(clientWidget);

        QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
        mainLayout->addWidget(splitter);

        connect(generalSendButton, &QPushButton::clicked, this, [this]()
                {
            std::string msg = generalInputField->text().toStdString();
            set_msg_to_general_chat("Me: " + msg);
            send_msg_to_all_clients(msg); });

        connect(clientList, &QListWidget::itemClicked, this, [this](QListWidgetItem *item)
                {
            if (name_table[item->text().toStdString()]->valid)
                name_table[item->text().toStdString()]->exec();
            else
                remove_client(item->text().toStdString()); });
    }

    ~MainWindow() = default;

    void add_client(int client_pid)
    {
        ++counter;
        QListWidgetItem *newClientItem = new QListWidgetItem("Client " + QString::number(counter));
        clientList->addItem(newClientItem);

        ChatWindow *chatWindow = new ChatWindow(client_pid, this);
        chatWindow->setWindowTitle(newClientItem->text());

        pid_table.emplace(client_pid, chatWindow);
        name_table.emplace(newClientItem->text().toStdString(), chatWindow);
        pid_name_table.emplace(client_pid, newClientItem->text().toStdString());
        name_pid_table.emplace(newClientItem->text().toStdString(), client_pid);
    }

    void remove_client(int client_pid)
    {
        if (pid_name_table.contains(client_pid))
        {
            std::string name = pid_name_table[client_pid];
            remove_client_by(client_pid, name);
        }
    }
    void remove_client(const std::string &name)
    {
        if(name_pid_table.contains(name))
        {
            int client_pid = name_pid_table[name];
            remove_client_by(client_pid, name);
        }
    }

    void set_msg_to_chat(int client_pid, const std::string &msg)
    {
        if (pid_table.contains(client_pid))
        {
            pid_table[client_pid]->set_text(msg);
        }
    }

    void set_msg_to_general_chat(const std::string &msg)
    {
        generalChatDisplay->append(QString::fromStdString(msg));
    }

    void send_msg_to_all_clients(const std::string &msg);

private:
    QListWidget *clientList;
    QTextEdit *generalChatDisplay;
    QLineEdit *generalInputField;
    QPushButton *generalSendButton;

    std::unordered_map<int, ChatWindow *> pid_table;
    std::unordered_map<std::string, ChatWindow *> name_table;
    std::unordered_map<int, std::string> pid_name_table;
    std::unordered_map<std::string, int> name_pid_table;

    int counter = 0;
    void remove_client_by(int client_pid, const std::string &name)
    {
        ChatWindow *chat = pid_table[client_pid];
        chat->close();
        delete chat;

        pid_table.erase(client_pid);
        name_table.erase(name);
        pid_name_table.erase(client_pid);
        name_pid_table.erase(name);

        for (int i = 0; i < clientList->count(); ++i)
        {
            QListWidgetItem *item = clientList->item(i);
            if (item->text().toStdString() == name)
            {
                delete clientList->takeItem(i);
                break;
            }
        }
    }
};
