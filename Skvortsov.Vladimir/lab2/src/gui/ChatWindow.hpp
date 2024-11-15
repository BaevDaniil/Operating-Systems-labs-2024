#ifndef CHATWINDOW_H
#define CHATWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

class ChatWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit ChatWindow(QWidget *parent = nullptr);
  ~ChatWindow();

private slots:
  void send_msg();

private:
  QTextEdit* chat_display;
  QLineEdit* msg_input;
  QPushButton* send_btn;

  void setup_ui();
};

#endif
