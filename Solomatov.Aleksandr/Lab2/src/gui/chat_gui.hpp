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
#include <QLabel>
#include <QTimer>
#include <QDialog>
#include "../includes/includes.hpp"

class ChatWindow : public QDialog {
    Q_OBJECT
public:
    ChatWindow(int client_pid, QWidget* parent = nullptr) : QDialog(parent), pid(client_pid) {
        setWindowTitle("Private Chat with Client " + QString::number(client_pid));
        QVBoxLayout* layout = new QVBoxLayout(this);

        chatDisplay = new QTextEdit(this);
        chatDisplay->setReadOnly(true);

        inputField = new QLineEdit(this);
        sendButton = new QPushButton("Send", this);

        QHBoxLayout* inputLayout = new QHBoxLayout();
        inputLayout->addWidget(inputField);
        inputLayout->addWidget(sendButton);

        layout->addWidget(chatDisplay);
        layout->addLayout(inputLayout);

        connect(sendButton, &QPushButton::clicked, this, [this]() {
            std::string msg = inputField->text().toStdString();
            emit send_private_msg(pid, msg);
            inputField->clear();
        });

        resize(500, 400);
    }

    void append_message(const std::string& msg) {
        chatDisplay->append(QString::fromStdString(msg));
    }

signals:
    void send_private_msg(int client_pid, const std::string& msg);

private:
    int pid;
    QTextEdit* chatDisplay;
    QLineEdit* inputField;
    QPushButton* sendButton;
};

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    public:
    MainWindow(const std::string& title, bool isHost, QWidget* parent = nullptr) 
        : QMainWindow(parent) {
        QWidget* centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);

        // Установка заголовка окна
        setWindowTitle(QString::fromStdString(title));

        QSplitter* splitter = new QSplitter(this);
        QVBoxLayout* generalChatLayout = new QVBoxLayout();
        generalChatDisplay = new QTextEdit(this);
        generalChatDisplay->setReadOnly(true);

        generalInputField = new QLineEdit(this);
        generalSendButton = new QPushButton("Send to All", this);

        QHBoxLayout* generalInputLayout = new QHBoxLayout();
        generalInputLayout->addWidget(generalInputField);
        generalInputLayout->addWidget(generalSendButton);

        generalChatLayout->addWidget(generalChatDisplay);
        generalChatLayout->addLayout(generalInputLayout);
        QWidget* generalChatWidget = new QWidget(this);
        generalChatWidget->setLayout(generalChatLayout);
        splitter->addWidget(generalChatWidget);

        if (isHost) {
            QVBoxLayout* clientLayout = new QVBoxLayout();
            clientList = new QListWidget(this);
            clientLayout->addWidget(clientList);
            QWidget* clientWidget = new QWidget(this);
            clientWidget->setLayout(clientLayout);
            splitter->addWidget(clientWidget);

            setWindowTitle("Host Chat Application");
        } else {
            setWindowTitle("Client Chat Application");
        }

        QHBoxLayout* mainLayout = new QHBoxLayout(centralWidget);
        mainLayout->addWidget(splitter);
        resize(800, 600);

        connect(generalSendButton, &QPushButton::clicked, this, &MainWindow::send_to_all_clients);
    }


public slots:
    void add_client(int client_pid) {
        QListWidgetItem* item = new QListWidgetItem("Client " + QString::number(client_pid));
        clientList->addItem(item);

        auto* chatWindow = new ChatWindow(client_pid, this);
        pid_to_chatWindow[client_pid] = chatWindow;

        connect(chatWindow, &ChatWindow::send_private_msg, this, &MainWindow::send_to_client);
    }

    void remove_client(int client_pid) {
        if (pid_to_chatWindow.contains(client_pid)) {
            delete pid_to_chatWindow[client_pid];
            pid_to_chatWindow.erase(client_pid);
        }
        for (int i = 0; i < clientList->count(); ++i) {
            QListWidgetItem* item = clientList->item(i);
            if (item->text() == "Client " + QString::number(client_pid)) {
                delete clientList->takeItem(i);
                break;
            }
        }
    }
    void set_msg_to_general_chat(const std::string& msg) {
        display_general_message(msg);
    }
    void display_general_message(const std::string& msg) {
        generalChatDisplay->append(QString::fromStdString(msg));
    }

    void display_private_message(int client_pid, const std::string& msg) {
        if (pid_to_chatWindow.contains(client_pid)) {
            pid_to_chatWindow[client_pid]->append_message(msg);
        }
    }

private slots:
    void send_to_all_clients() {
        std::string msg = generalInputField->text().toStdString();
        generalInputField->clear();
        display_general_message(msg);
        emit broadcast_message(msg);
    }

    void send_to_client(int client_pid, const std::string& msg) {
        emit send_private_message(client_pid, msg);
    }

    void check_client_activity() {
        emit check_inactive_clients();
    }

signals:
    void broadcast_message(const std::string& msg);
    void send_private_message(int client_pid, const std::string& msg);
    void check_inactive_clients();

private:
    QTextEdit* generalChatDisplay;
    QLineEdit* generalInputField;
    QPushButton* generalSendButton;
    QListWidget* clientList;
    QTimer* clientTimer;
    std::unordered_map<int, ChatWindow*> pid_to_chatWindow;
};
