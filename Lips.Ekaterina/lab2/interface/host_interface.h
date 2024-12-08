#pragma once

#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QListWidget>
#include <QString>
#include <vector>
#include <string>
#include <QWidget>
#include "../book.h"

class HostWindow : public QMainWindow {
    Q_OBJECT
    
private:
    QLabel* left_label;
    QLabel* right_label;
    QListWidget* book_qlist;
    QListWidget* history_list;

    // QPushButton* terminateClientButton;
    // QPushButton* terminateHostButton;
    // int hostPort;

public:
    HostWindow(const std::vector<Book>& books, QWidget* parent = nullptr);
    virtual ~HostWindow();

    void update_books(const std::vector<Book>& books, std::string state, std::string book_name, std::string client_name, std::string time, bool flag);


// private slots:
//     void terminateClient();
//     void terminateHost();


};