#include "ChatWindow.hpp"

ChatWindow::ChatWindow(QWidget* parent) : QMainWindow(parent) {
  setup_ui();
};

ChatWindow::~ChatWindow() {
  // Здесь можно добавить код для очистки ресурсов, если необходимо
};

void ChatWindow::setup_ui() {
  QWidget* central_widget = new QWidget(this);
  setCentralWidget(central_widget);

  // Создаем и настраиваем виджеты
  chat_display = new QTextEdit(this);
  chat_display->setReadOnly(true);

  msg_input = new QLineEdit(this);

  send_btn = new QPushButton("Send", this);

  // Создаем и настраиваем компоновку
  QVBoxLayout* mainLayout = new QVBoxLayout();
  QHBoxLayout* inputLayout = new QHBoxLayout();

  inputLayout->addWidget(msg_input);
  inputLayout->addWidget(send_btn);

  mainLayout->addWidget(chat_display);
  mainLayout->addLayout(inputLayout);

  central_widget->setLayout(mainLayout);

  // Подключаем сигнал нажатия кнопки к слоту отправки сообщения
  connect(send_btn, &QPushButton::clicked, this, &ChatWindow::send_msg);
  // Также подключаем нажатие клавиши Enter в поле ввода к отправке сообщения
  connect(msg_input, &QLineEdit::returnPressed, this, &ChatWindow::send_msg);
}

void ChatWindow::send_msg() {
  QString message = msg_input->text().trimmed();
  if (message.isEmpty()) {
    QMessageBox::warning(this, "Warning", "Cannot send an empty message.");
    return;
  }

  // Здесь вы можете добавить код для отправки сообщения через выбранный механизм IPC
  // Например, через сокет, очередь сообщений или именованный канал

  // Отображаем отправленное сообщение в чате
  chat_display->append("Me: " + message);

  // Очищаем поле ввода
  msg_input->clear();
}
