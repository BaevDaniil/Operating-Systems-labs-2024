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
  void sendMessage();

private:
  QTextEdit *chatDisplay; // Виджет для отображения чата
  QLineEdit *messageInput; // Поле ввода сообщения
  QPushButton *sendButton; // Кнопка отправки сообщения

  void setupUi(); // Метод для настройки пользовательского интерфейса
};

#endif // CHATWINDOW_H
