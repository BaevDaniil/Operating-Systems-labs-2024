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
#include <iostream>
#include <vector>
#include <string>
#include <csignal>



class ClientWindow : public QMainWindow {
    Q_OBJECT
private:

    constexpr static int conn_timeout = 10;
    int timekill = conn_timeout;

    pid_t client_pid;

    QStackedWidget* stacked_widget; 

    QListWidget* book_qlist;
    QPushButton* select_button;

    QLabel* reading_label;
    QPushButton* cancel_reading_button;

    QLabel* timer_label;
    QLabel* book_list_label;
    QLabel* history_list_label;
    QListWidget* history_list;
    
    QTimer timer;

    void create_base_view(const std::vector<Book>& books);
    void create_reading_view();

public:

    ClientWindow(const std::vector<Book>& books, pid_t pid, QWidget* parent = nullptr);
    ~ClientWindow();

    void success_take_book();
    void fail_take_book();
    void update_books(const std::vector<Book>& books, std::string state, std::string book_name, std::string time, bool flag);

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