#pragma once

#include "../book.h"
#include "../connection/conn.h"

#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QListWidget>
#include <QStackedWidget>
#include <QString>
#include <QWidget>
#include <QMessageBox>
#include <QTimer>
#include <algorithm>
#include <iostream>
#include <vector>
#include <string>
#include <csignal>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <csignal>


class ClientWindow : public QMainWindow {
    Q_OBJECT
private:

    constexpr static int conn_timeout = 10;
    int timekill = conn_timeout;

    pid_t client_pid;

    std::vector<std::string> reading_books;

    QListWidget* book_qlist;
    QPushButton* select_button;
    QListWidget* reading_qlist;
    QPushButton* cancel_button;
    QListWidget* history_list;

    QLabel* reading_label;
    QLabel* timer_label;
    QLabel* book_list_label;
    QLabel* history_list_label;
    
    QTimer timer;

    void create_base_view(const std::vector<Book>& books);

public:

    ClientWindow(const std::vector<Book>& books, pid_t pid, QWidget* parent = nullptr);
    ~ClientWindow();

    void success_take_book(const std::string& book_name);
    void fail_take_book();
    void update_history(const std::vector<Book>& books, const std::string& state, const std::string& book_name, const std::string& time, bool flag);

    void restart_timer_wrap();
    void stop_timer_wrap();

signals:

    void book_selected_signal(const QString& book_name);
    void book_returned_signal(const QString& book_name);

    void restart_timer_signal();
    void stop_timer_signal();

private slots:

    void select_book();
    void cancel_reading();  

    void terminate_client();
    void restart_timer();
};