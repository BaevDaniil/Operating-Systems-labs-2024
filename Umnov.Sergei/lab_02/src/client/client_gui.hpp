#ifndef CLIENT_GUI_HPP
#define CLIENT_GUI_HPP

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtCore/QTimer>
#include "conn_seg.hpp"
#include "conn_mq.hpp"
#include "conn_fifo.hpp"

class ClientGUI : public QMainWindow {
    Q_OBJECT

public:
    ClientGUI(QWidget *parent = nullptr);
    ~ClientGUI();

public slots:
    void onSendButtonClicked();
    void checkActivity();

private:
    QTextEdit *chatDisplay;
    QLineEdit *messageInput;
    QPushButton *sendButton;
    QVBoxLayout *layout;
    QWidget *centralWidget;
    ConnSeg clientSegConn;
    ConnMq clientMqConn;
    ConnFifo clientFifoConn;
    QTimer *activityTimer;

    void sendMessage(const QString &message);
    void receiveMessage();
};

#endif // CLIENT_GUI_HPP
