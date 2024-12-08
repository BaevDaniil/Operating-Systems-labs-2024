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
#include <vector>
#include <string>



class ClientWindow : public QMainWindow {
    Q_OBJECT
private:
    void create_base_view(const std::vector<Book>& books);
    void create_reading_view();

    QStackedWidget* stacked_widget; 

    QListWidget* book_qlist;
    QPushButton* select_button;

    QLabel* reading_label;
    QPushButton* cancel_reading_button;

    QLabel* left_label;
    QLabel* right_label;
    QListWidget* history_list;

public:
    ClientWindow(const std::vector<Book>& books, QWidget* parent = nullptr);
    ~ClientWindow();

    void success_take_book();
    void fail_take_book();
    void update_books(const std::vector<Book>& books, std::string state, std::string book_name, std::string time, bool flag);

signals:
    void bookSelected(const QString& book_name);
    void bookReturned(const QString& book_name);

private slots:
    void select_book();
    void cancel_reading();

    // QPushButton* terminateHostButton;
};