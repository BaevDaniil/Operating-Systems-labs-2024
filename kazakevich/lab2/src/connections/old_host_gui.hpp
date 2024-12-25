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
class ChatWindow : public QDialog
{
    Q_OBJECT
public:
    ChatWindow(QWidget *parent = nullptr) : QDialog(parent)
    {
        setWindowTitle("Chat");

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
            send_msg(msg);
        });
    }

    ~ChatWindow() = default;
private:
    int pid;
    QVBoxLayout *layout;
    QTextEdit *chatDisplay;
    QPushButton *sendButton;
    QLineEdit *inputField;
    QHBoxLayout *inputLayout;
public:
    bool valid = true;
    void set_text(const std::string& msg)
    {
        chatDisplay->append(QString::fromStdString(msg));
    }

    void send_msg(const std::string& msg);
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr) : QMainWindow(parent)
    {
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
        clientList = new QListWidget(this);

        QLabel *ChatLabel = new QLabel("List of clients", this);
        ChatLabel->setAlignment(Qt::AlignCenter);
        clientLayout->addWidget(ChatLabel);

        clientLayout->addWidget(clientList);

        splitter->addWidget(generalChatWidget);
        splitter->addWidget(clientWidget);

        QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
        mainLayout->addWidget(splitter);
        setWindowTitle("Host chat Application");
        resize(1200, 900);
        connect(generalSendButton, &QPushButton::clicked, this, [this]()
        {
            std::string msg = generalInputField->text().toStdString();
            set_msg_to_general_chat("my: " + msg);
            send_msg_to_all_clients(msg);
        });
        connect(clientList, &QListWidget::itemClicked, this, [this](QListWidgetItem *item)
        {
            if (name_table[item->text().toStdString()]->valid)
                name_table[item->text().toStdString()]->exec();
            else
                remove_client(item->text().toStdString());
        });
    }

    ~MainWindow() = default;

public slots:
    void slot_add_client(int client_pid)
    {
        add_client(client_pid);
    }
private:
    QListWidget *clientList;
    QTextEdit *generalChatDisplay;
    std::unordered_map<int, ChatWindow*> pid_table;
    std::unordered_map<std::string, ChatWindow*> name_table;
    std::unordered_map<int, std::string> pid_name_table;
    std::unordered_map<std::string, int> name_pid_table;
    QLineEdit *generalInputField;
    QPushButton *generalSendButton;

    void remove_client_by(int client_pid, const std::string& name)
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

public :
    void add_client(int client_pid)
    {
        QListWidgetItem *newClientItem = new QListWidgetItem("Client " + QString::number(clientList->count() + 1));
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
        std::string name = pid_name_table[client_pid];
        remove_client_by(client_pid, name);
    }

    void remove_client(const std::string& name)
    {
        int client_pid = name_pid_table[name];
        remove_client_by(client_pid, name);
    }

    void set_msg_to_chat(int client_pid, const std::string& msg)
    {
        pid_table[client_pid]->set_text(msg);
    }

    void set_msg_to_general_chat(const std::string &msg)
    {
        generalChatDisplay->append(QString::fromStdString(msg));
    }

    void send_msg_to_all_clients(const std::string &msg);
};
