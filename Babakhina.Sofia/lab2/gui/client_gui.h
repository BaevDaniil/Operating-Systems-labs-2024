#pragma once

#include <QMainWindow>
#include <QLabel>
#include <QObject>
#include <QTimer>
#include <QPushButton>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QListWidget>
#include <QStackedWidget>
#include <QString>
#include <QWidget>
#include <QTableWidget>
#include <QStringList>
#include <QHeaderView>
#include <QMessageBox>

#include <fstream>
#include <iostream>
#include <unistd.h>
#include <sys/ipc.h>
#include <thread>
#include <stdlib.h>
#include <csignal>
#include <vector>
#include <string>

#include "../book.h"
#include "../connection/conn.h"


class ClientWindow : public QMainWindow {
    Q_OBJECT
private:
    void create_general_view(const std::vector<Book>& books);
    void create_reading_view();

    QStackedWidget* stacked_widget; 
    
    QLabel* reading_label;
    QLabel* left_label;
    QLabel* right_label;
    QLabel* timer_label;

    QListWidget* history_list;
    QTableWidget* table;
    QTableWidget* book_table;

    QPushButton* select_button;
    QPushButton* cancel_reading_button;

public:
    pid_t client_pid = 0;

    ClientWindow(const std::vector<Book>& books, QWidget* parent = nullptr);
    ~ClientWindow();

    void success_take_book();
    void fail_take_book();
    void update_books(const std::vector<Book>& books, std::string state, std::string book_name, std::string time, bool flag);
    
    void wrap_reset_timer();
    void wrap_stop_timer();

    QTimer timer;
    constexpr static int conn_timeout = 10;
    int timekill = conn_timeout;

signals:
    void book_selected(const QString& book_name);
    void book_returned(const QString& book_name);

    void signal_reset_timer();
    void signal_stop_timer();

private slots:
    void select_book();
    void cancel_reading();

    void terminate_client();
    void reset_timer();
};
