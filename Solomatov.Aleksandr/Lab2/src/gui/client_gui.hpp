#pragma once
#include "../includes/includes.hpp"

class ClientMainWindow : public QMainWindow {
    Q_OBJECT

public:
    ClientMainWindow(int client_pid, QWidget *parent = nullptr) : QMainWindow(parent), pid(client_pid) {
        setWindowTitle("Client Chat Application");
        resize(1200, 900);

        // Общий стиль для всего окна
        setStyleSheet(
            "QMainWindow { background-color: #f0f0f5; }"
            "QTextEdit { background-color: #ffffff; border: 1px solid #dcdcdc; border-radius: 8px; font-size: 14px; }"
            "QLineEdit { background-color: #ffffff; border: 1px solid #dcdcdc; border-radius: 8px; padding: 6px; font-size: 14px; }"
            "QPushButton { background-color: #007BFF; color: white; border-radius: 8px; padding: 10px 20px; font-size: 14px; }"
            "QPushButton:hover { background-color: #0056b3; }"
            "QLabel { font-weight: bold; font-size: 16px; color: #333333; margin-bottom: 8px; }");

        // Центральный виджет
        QWidget *centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);

        QSplitter *splitter = new QSplitter(Qt::Horizontal, this);
        splitter->setHandleWidth(2);
        splitter->setStyleSheet("QSplitter::handle { background-color: #cccccc; }");

        
        QWidget *generalChatWidget = createChatWidget("General Chat", generalChatDisplay, generalInputField, generalSendButton);
        QWidget *privateChatWidget = createChatWidget("Private Chat with Host", ChatDisplay, InputField, SendButton);

        
        splitter->addWidget(generalChatWidget);
        splitter->addWidget(privateChatWidget);
        splitter->setStretchFactor(0, 1);
        splitter->setStretchFactor(1, 1);

        QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
        mainLayout->addWidget(splitter);

        connect(generalSendButton, &QPushButton::clicked, this, [this]() {
            QString msg = generalInputField->text();
            if (!msg.isEmpty()) {
                display_message("Me: [" + QString::number(pid) + "] " + msg, generalChatDisplay);
                send_to_all(msg.toStdString());
                generalInputField->clear();
            }
        });

        connect(SendButton, &QPushButton::clicked, this, [this]() {
            QString msg = InputField->text();
            if (!msg.isEmpty()) {
                display_message("Me: " + msg, ChatDisplay);
                send_to_host(msg.toStdString());
                InputField->clear();
            }
        });
    }

    ~ClientMainWindow() override = default;

private:
    int pid;
    QTextEdit *generalChatDisplay;
    QLineEdit *generalInputField;
    QPushButton *generalSendButton;

    QTextEdit *ChatDisplay;      
    QLineEdit *InputField;       
    QPushButton *SendButton;     

    QWidget *createChatWidget(const QString &title, QTextEdit *&display, QLineEdit *&input, QPushButton *&sendButton) {
        QWidget *chatWidget = new QWidget(this);
        QVBoxLayout *chatLayout = new QVBoxLayout(chatWidget);
        chatLayout->setSpacing(10);
        chatLayout->setContentsMargins(10, 10, 10, 10);

        QLabel *chatLabel = new QLabel(title, this);
        chatLabel->setAlignment(Qt::AlignCenter);
        chatLayout->addWidget(chatLabel);

        display = new QTextEdit(this);
        display->setReadOnly(true);
        chatLayout->addWidget(display);

        input = new QLineEdit(this);
        sendButton = new QPushButton("Send", this);

        QHBoxLayout *inputLayout = new QHBoxLayout();
        inputLayout->addWidget(input);
        inputLayout->addWidget(sendButton);
        chatLayout->addLayout(inputLayout);

        return chatWidget;
    }

    void display_message(const QString &msg, QTextEdit *chat_to_display) {
        chat_to_display->append(msg);
    }

public:
    void set_message_on_chat(const std::string &msg) {
        display_message(QString::fromStdString(msg), ChatDisplay);
    }

    void setMessageToGeneralChat(const std::string &msg) {
        display_message(QString::fromStdString(msg), generalChatDisplay);
    }

    void send_to_all(const std::string &msg);
    void send_to_host(const std::string &msg);
};
