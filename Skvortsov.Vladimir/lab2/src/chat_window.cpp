#include <QMessageBox>
#include <QApplication>
#include <QTimer>
#include "chat_window.hpp"

ChatWindow::ChatWindow(QWidget* parent) : QMainWindow(parent), host_conn(nullptr), client_conn(nullptr) {
  setup_ui();
  setup_conn();
  setup_timers();
};

ChatWindow::~ChatWindow() {
  if (host_conn) {
    delete host_conn;
  }
  if (client_conn) {
    delete client_conn;
  }
};

void ChatWindow::setup_ui() {
  QWidget* central_widget = new QWidget(this);
  setCentralWidget(central_widget);

  chat_display = new QTextEdit(this);
  chat_display->setReadOnly(true);

  msg_input = new QLineEdit(this);
  send_btn = new QPushButton("Send", this);

  QVBoxLayout* mainLayout = new QVBoxLayout();
  QHBoxLayout* inputLayout = new QHBoxLayout();

  inputLayout->addWidget(msg_input);
  inputLayout->addWidget(send_btn);

  mainLayout->addWidget(chat_display);
  mainLayout->addLayout(inputLayout);

  central_widget->setLayout(mainLayout);

  connect(send_btn, &QPushButton::clicked, this, &ChatWindow::send_msg);
  connect(msg_input, &QLineEdit::returnPressed, this, &ChatWindow::send_msg);
};

void ChatWindow::setup_timers() {
  QTimer* timer = new QTimer(this);
  connect(timer, &QTimer::timeout, this, &ChatWindow::read_msg);
  timer->start(10);
};
