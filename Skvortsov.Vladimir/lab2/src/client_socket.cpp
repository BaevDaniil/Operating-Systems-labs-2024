#include <QMessageBox>
#include <QApplication>
#include <QTimer>
#include <fstream>
#include <unistd.h>
#include "conn_socket.hpp"
#include "ChatWindow.hpp"

const std::string SERVER_ADDRESS = "127.0.0.1";
const int SERVER_PORT = 8080;

void ChatWindow::setup_conn() {
  ConnSocket* client_conn = new ConnSocket();

  if (!client_conn->connect_to_server(SERVER_ADDRESS, SERVER_PORT)) {
    QMessageBox::critical(this, "Error", "Error connecting to server\n");
    return;
  }

  this->client_conn = client_conn;
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
  if (client_conn && client_conn->is_valid()) {
    std::string msg;
    const size_t max_size = 1024;
    if (client_conn->read(msg, max_size)) {
      if (!msg.empty()) {
        chat_display->append(">>> " + QString::fromStdString(msg));
      }
    }
  }
};

int main(int argc, char*argv[]) {
  QApplication app(argc, argv);

  ChatWindow window;
  window.setWindowTitle("Chat Application");
  window.resize(400, 300);
  window.show();

  return app.exec();
};
