#include <QMessageBox>
#include <QApplication>
#include <QTimer>
#include <fstream>
#include <unistd.h>
#include "conn_queue.hpp"
#include "ChatWindow.hpp"

void ChatWindow::setup_conn() {
  pid_t pid = getpid();
  std::ofstream pid_file("host_pid.txt");
  if (!pid_file) {
    QMessageBox::critical(this, "Error", "Failed to open PID file for writing");
    return;
  }
  pid_file << pid;
  pid_file.close();

  std::string client_path = "/chat_client_queue" + std::to_string(pid);
  std::string host_path = "/chat_host_queue" + std::to_string(pid);

  client_conn = new ConnQueue(client_path, true);
  if (!client_conn->is_valid()) {
    QMessageBox::critical(this, "Error", "Failed to open FIFO for writing");
    return;
  }

  host_conn = new ConnQueue(host_path, true);
  if (!host_conn->is_valid()) {
    QMessageBox::critical(this, "Error", "Failed to open FIFO for reading");
    return;
  }
};

void ChatWindow::send_msg() {
  QString msg = msg_input->text().trimmed();
  if (msg.isEmpty()) {
    QMessageBox::warning(this, "Warning", "Cannot send an empty message.");
    return;
  }

  if (client_conn && client_conn->is_valid()) {
    if (!client_conn->write(msg.toStdString())) {
      QMessageBox::critical(this, "Error", "Failed to send message");
      return;
    }
    chat_display->append("<<< " + msg);
  }

  msg_input->clear();
};

void ChatWindow::read_msg() {
  if (host_conn && host_conn->is_valid()) {
    std::string msg;
    const size_t max_size = 1024;
    if (host_conn->read(msg, max_size)) {
      if (!msg.empty()) {
        chat_display->append(">>> " + QString::fromStdString(msg));
      }
    }
  }
};

int main(int argc, char* argv[]) {
  QApplication app(argc, argv);

  ChatWindow window;
  window.setWindowTitle("Chat Application");
  window.resize(400, 300);
  window.show();

  return app.exec();
};

