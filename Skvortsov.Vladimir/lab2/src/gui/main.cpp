#include <QApplication>
#include "ChatWindow.hpp"

int main(int argc, char* argv[]) {
  QApplication app(argc, argv);

  ChatWindow window;
  window.setWindowTitle("Chat Application");
  window.resize(400, 300);
  window.show();

  return app.exec();
}
