#pragma once
#include "../includes/includes.hpp"

class ChatWindow : public QDialog {
    Q_OBJECT

public:
    ChatWindow(int client_pid, QWidget *parent = nullptr)
        : QDialog(parent), pid(client_pid) {
        setWindowTitle("Chat with Client " + QString::number(client_pid));
        resize(600, 400);

        setStyleSheet(
            "QDialog { background-color: #f0f0f5; }"
            "QTextEdit { background-color: #ffffff; border: 1px solid #dcdcdc; border-radius: 8px; font-size: 14px; }"
            "QLineEdit { background-color: #ffffff; border: 1px solid #dcdcdc; border-radius: 8px; padding: 6px; font-size: 14px; }"
            "QPushButton { background-color: #007BFF; color: white; border-radius: 8px; padding: 10px 20px; font-size: 14px; }"
            "QPushButton:hover { background-color: #0056b3; }"
            "QLabel { font-weight: bold; font-size: 16px; color: #333333; }");

        QVBoxLayout *mainLayout = new QVBoxLayout(this);

        chatDisplay = new QTextEdit(this);
        chatDisplay->setReadOnly(true);

        inputField = new QLineEdit(this);
        sendButton = new QPushButton("Send", this);

        QHBoxLayout *inputLayout = new QHBoxLayout();
        inputLayout->addWidget(inputField);
        inputLayout->addWidget(sendButton);

        mainLayout->addWidget(chatDisplay);
        mainLayout->addLayout(inputLayout);

        // Слот для отправки сообщения
        connect(sendButton, &QPushButton::clicked, this, [this]() {
            QString msg = inputField->text();
            if (!msg.isEmpty()) {
                emit messageSent(pid, msg.toStdString());
                inputField->clear();
                set_text("Me: " +  msg.toStdString());
            }

        });
    }

    void set_text(const std::string &msg) {
        QMetaObject::invokeMethod(this, [this, msg]() {
            chatDisplay->append(QString::fromStdString(msg));
        }, Qt::QueuedConnection);
    }

signals:
    void messageSent(int client_pid, const std::string &msg);

private:
    int pid;
    QTextEdit *chatDisplay;
    QLineEdit *inputField;
    QPushButton *sendButton;
};

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr) : QMainWindow(parent) {
        setWindowTitle("Host Chat Application");
        resize(1000, 700);

        setStyleSheet(
            "QMainWindow { background-color: #f0f0f5; }"
            "QTextEdit { background-color: #ffffff; border: 1px solid #dcdcdc; border-radius: 8px; font-size: 14px; }"
            "QLineEdit { background-color: #ffffff; border: 1px solid #dcdcdc; border-radius: 8px; padding: 6px; font-size: 14px; }"
            "QPushButton { background-color: #007BFF; color: white; border-radius: 8px; padding: 10px 20px; font-size: 14px; }"
            "QPushButton:hover { background-color: #0056b3; }"
            "QLabel { font-weight: bold; font-size: 16px; color: #333333; }"
            "QListWidget { background-color: #ffffff; border: 1px solid #dcdcdc; border-radius: 8px; font-size: 14px; }");

        QWidget *centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);

        QSplitter *splitter = new QSplitter(Qt::Horizontal, this);
        splitter->setHandleWidth(2);
        splitter->setStyleSheet("QSplitter::handle { background-color: #cccccc; }");

        // Общий чат
        QWidget *generalChatWidget = new QWidget(this);
        QVBoxLayout *generalChatLayout = new QVBoxLayout(generalChatWidget);
        generalChatLayout->setSpacing(10);
        generalChatLayout->setContentsMargins(10, 10, 10, 10);

        QLabel *generalChatLabel = new QLabel("General Chat", this);
        generalChatLabel->setAlignment(Qt::AlignCenter);
        generalChatLayout->addWidget(generalChatLabel);

        generalChatDisplay = new QTextEdit(this);
        generalChatDisplay->setReadOnly(true);
        generalChatLayout->addWidget(generalChatDisplay);

        generalInputField = new QLineEdit(this);
        generalSendButton = new QPushButton("Send", this);

        QHBoxLayout *generalInputLayout = new QHBoxLayout();
        generalInputLayout->addWidget(generalInputField);
        generalInputLayout->addWidget(generalSendButton);
        generalChatLayout->addLayout(generalInputLayout);

        splitter->addWidget(generalChatWidget);

        // Список клиентов
        QWidget *clientListWidget = new QWidget(this);
        QVBoxLayout *clientLayout = new QVBoxLayout(clientListWidget);
        clientLayout->setSpacing(10);
        clientLayout->setContentsMargins(10, 10, 10, 10);

        QLabel *clientListLabel = new QLabel("Clients", this);
        clientListLabel->setAlignment(Qt::AlignCenter);
        clientLayout->addWidget(clientListLabel);

        clientList = new QListWidget(this);
        clientLayout->addWidget(clientList);

        splitter->addWidget(clientListWidget);

        QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
        mainLayout->addWidget(splitter);

        connect(generalSendButton, &QPushButton::clicked, this, [this]() {
            QString msg = generalInputField->text();
            if (!msg.isEmpty()) {
                set_msg_to_general_chat("Host: " + msg.toStdString());
                emit send_to_all_clients(msg.toStdString());
                generalInputField->clear();
            }
        });

        connect(clientList, &QListWidget::itemClicked, this, [this](QListWidgetItem *item) {
            QString clientInfo = item->text();
            int client_pid = clientInfo.split(" ").last().toInt();
            open_private_chat(client_pid);
        });
    }

    void set_msg_to_general_chat(const std::string &msg) {
        QMetaObject::invokeMethod(this, [this, msg]() {
            generalChatDisplay->append(QString::fromStdString(msg));
        }, Qt::QueuedConnection);
    }

    void set_msg_to_private_chat(int client_pid, const std::string &msg) {
        std::lock_guard<std::mutex> lock(map_mutex);

        message_history[client_pid].push_back(msg);

        if (pid_table.contains(client_pid)) {
            pid_table[client_pid]->set_text(msg);
        }
    }

    void add_client(int client_pid) {
        std::lock_guard<std::mutex> lock(map_mutex);

        if (pid_table.contains(client_pid))
            return;

        QListWidgetItem *newItem = new QListWidgetItem("Client " + QString::number(client_pid));
        clientList->addItem(newItem);

        message_history[client_pid] = {};
    }

    void open_private_chat(int client_pid) {
        std::lock_guard<std::mutex> lock(map_mutex);

        if (!pid_table.contains(client_pid)) {
            auto *chatWindow = new ChatWindow(client_pid, this);
            pid_table[client_pid] = chatWindow;

            connect(chatWindow, &ChatWindow::messageSent, this, [this](int pid, const std::string &msg) {
                emit send_to_client_private(pid, msg);
            });

            connect(chatWindow, &QDialog::finished, this, [this, client_pid]() {
                std::lock_guard<std::mutex> lock(map_mutex);
                if (pid_table.contains(client_pid)) {
                    pid_table.erase(client_pid);
                }
            });

            for (const auto &msg : message_history[client_pid]) {
                chatWindow->set_text(msg);
            }
        }

        pid_table[client_pid]->show();
        pid_table[client_pid]->raise(); 
        pid_table[client_pid]->activateWindow();
    }

    void remove_client(int client_pid) {
        std::lock_guard<std::mutex> lock(map_mutex);
        if (pid_table.contains(client_pid)) {
            auto chatWindow = pid_table[client_pid];
            chatWindow->close();
            delete chatWindow;
            pid_table.erase(client_pid);
        }
        message_history.erase(client_pid);
    }

    void send_to_all_clients(const std::string &msg);
    void send_to_client_private(int client_pid, const std::string &msg);

private:
    QTextEdit *generalChatDisplay;
    QLineEdit *generalInputField;
    QPushButton *generalSendButton;
    QListWidget *clientList;

    std::unordered_map<int, ChatWindow *> pid_table;
    std::unordered_map<int, std::vector<std::string>> message_history;
    std::mutex map_mutex;
};

