#include <QMessageBox>
#include <QApplication>
#include <QTimer>
#include <fstream>
#include <unistd.h>
#include "conn_fifo.hpp"
#include "ChatWindow.hpp"

ChatWindow::ChatWindow(QWidget* parent) : QMainWindow(parent), host_fifo(nullptr), client_fifo(nullptr) {
  setup_ui();
  setup_conn();
  setup_timers();
};

ChatWindow::~ChatWindow() {
  if (host_fifo) {
    delete host_fifo;
  }
  if (client_fifo) {
    delete client_fifo;
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

void ChatWindow::setup_conn() {
  pid_t pid = getpid();
  std::ofstream pid_file("host_pid.txt");
  if (!pid_file) {
    QMessageBox::critical(this, "Error", "Failed to open PID file for writing");
    return;
  }
  pid_file << pid;
  pid_file.close();

  std::string client_path = "/tmp/chat_client_fifo" + std::to_string(pid);
  std::string host_path = "/tmp/chat_host_fifo" + std::to_string(pid);

  client_fifo = new ConnFifo(client_path, true);
  if (!client_fifo->is_valid()) {
    QMessageBox::critical(this, "Error", "Failed to open FIFO for writing");
    return;
  }

  host_fifo = new ConnFifo(host_path, true);
  if (!host_fifo->is_valid()) {
    QMessageBox::critical(this, "Error", "Failed to open FIFO for reading");
    return;
  }
};

void ChatWindow::setup_timers() {
  QTimer* timer = new QTimer(this);
  connect(timer, &QTimer::timeout, this, &ChatWindow::read_msg);
  timer->start(10);
};

void ChatWindow::send_msg() {
  QString message = msg_input->text().trimmed();
  if (message.isEmpty()) {
    QMessageBox::warning(this, "Warning", "Cannot send an empty message.");
    return;
  }

  if (client_fifo && client_fifo->is_valid()) {
    if (!client_fifo->write(message.toStdString())) {
      QMessageBox::critical(this, "Error", "Failed to send message");
      return;
    }
    chat_display->append("<<< " + message);
  }

  msg_input->clear();
};

void ChatWindow::read_msg() {
  if (host_fifo && host_fifo->is_valid()) {
    std::string msg;
    const size_t max_size = 1024;
    if (host_fifo->read(msg, max_size)) {
      if (!msg.empty()) {
        chat_display->append(">>> " + QString::fromStdString(msg));
      }
    }
  }
};

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  ChatWindow window;
  window.setWindowTitle("Chat Application");
  window.resize(400, 300);
  window.show();

  return app.exec();
};
