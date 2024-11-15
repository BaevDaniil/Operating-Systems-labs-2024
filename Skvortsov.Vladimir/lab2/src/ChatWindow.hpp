#ifndef CHATWINDOW_H
#define CHATWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include "conn_fifo.hpp"

class ChatWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit ChatWindow(QWidget *parent = nullptr);
    ~ChatWindow();

private slots:
    void send_msg();
    void read_msg();

private:
    QTextEdit* chat_display;
    QLineEdit* msg_input;
    QPushButton* send_btn;

    ConnFifo* host_fifo;
    ConnFifo* client_fifo;

    void setup_ui();
    void setup_conn();
    void setup_timers();
};

#endif
