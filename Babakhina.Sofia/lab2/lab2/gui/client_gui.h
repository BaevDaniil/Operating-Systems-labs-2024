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

public:
    ClientWindow(const std::vector<Book>& books, QWidget* parent = nullptr);
    ~ClientWindow();

    void onSuccessTakeBook();
    void onFailedTakeBook();

signals:
    void bookSelected(const QString& bookName);
    void bookReturned(const QString& bookName);

private slots:
    void selectBook();
    void cancelReading();

    // void write_to_host();
    // void read_from_host();
    // void terminateClient();

private:
    void createBookView(const std::vector<Book>& books);
    void createReadingView();

    // void setup_ui();
    // void setup_conn();
    // void setup_timers();
        
    // Conn* host_conn;
    // Conn* client_conn;

    QStackedWidget* stackedWidget; // For switch windows (started <-> reading)

    QListWidget* bookList;
    QPushButton* selectButton;

    QLabel* readingLabel;
    QPushButton* cancelReadingButton;

    // QPushButton* terminateClientButton;


    QLabel* portLabel;
    QLabel* right_label;
    QListWidget* history_list;
    // QPushButton* terminateHostButton;
};