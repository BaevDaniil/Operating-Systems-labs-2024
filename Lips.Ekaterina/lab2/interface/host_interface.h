#pragma once

#include "../book.h"

#include <QMainWindow>
#include <QWidget>
#include <QMessageBox>
#include <QApplication>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QListWidget>
#include <QString>
#include <vector>
#include <string>
#include <cstdlib>
#include <signal.h>


class HostWindow : public QMainWindow {
    Q_OBJECT
    
private:

    QLabel* book_list_label;
    QLabel* history_list_label;
    QListWidget* book_qlist;
    QListWidget* history_list;

public:

    HostWindow(const std::vector<Book>& books, QWidget* parent = nullptr);
    virtual ~HostWindow();

    void update_history(const std::vector<Book>& books, const std::string& state, const std::string& book_name, const std::string& client_name, const std::string& time, bool flag);

};