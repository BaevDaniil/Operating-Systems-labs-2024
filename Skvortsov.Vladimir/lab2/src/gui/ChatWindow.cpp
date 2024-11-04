#include "ChatWindow.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>

ChatWindow::ChatWindow(QWidget *parent) : QMainWindow(parent) {
  setupUi();
}

ChatWindow::~ChatWindow() {
  // Здесь можно добавить код для очистки ресурсов, если необходимо
}

void ChatWindow::setupUi() {
  // Создаем центральный виджет и устанавливаем его в главное окно
  QWidget *centralWidget = new QWidget(this);
  setCentralWidget(centralWidget);

  // Создаем и настраиваем виджеты
  chatDisplay = new QTextEdit(this);
  chatDisplay->setReadOnly(true);

  messageInput = new QLineEdit(this);

  sendButton = new QPushButton("Send", this);

  // Создаем и настраиваем компоновку
  QVBoxLayout *mainLayout = new QVBoxLayout();
  QHBoxLayout *inputLayout = new QHBoxLayout();

  inputLayout->addWidget(messageInput);
  inputLayout->addWidget(sendButton);

  mainLayout->addWidget(chatDisplay);
  mainLayout->addLayout(inputLayout);

  centralWidget->setLayout(mainLayout);

  // Подключаем сигнал нажатия кнопки к слоту отправки сообщения
  connect(sendButton, &QPushButton::clicked, this, &ChatWindow::sendMessage);
  // Также подключаем нажатие клавиши Enter в поле ввода к отправке сообщения
  connect(messageInput, &QLineEdit::returnPressed, this, &ChatWindow::sendMessage);
}

void ChatWindow::sendMessage() {
  QString message = messageInput->text().trimmed();
  if (message.isEmpty()) {
    QMessageBox::warning(this, "Warning", "Cannot send an empty message.");
    return;
  }

  // Здесь вы можете добавить код для отправки сообщения через выбранный механизм IPC
  // Например, через сокет, очередь сообщений или именованный канал

  // Отображаем отправленное сообщение в чате
  chatDisplay->append("Me: " + message);

  // Очищаем поле ввода
  messageInput->clear();
}
