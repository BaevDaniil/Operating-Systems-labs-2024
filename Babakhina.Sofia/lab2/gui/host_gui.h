#pragma once

#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QListWidget>
#include <QString>
#include <QWidget>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QMessageBox>
#include <QApplication>
#include <QStringList>
#include <QHeaderView>

#include <cstdlib>
#include <signal.h>
#include <vector>
#include <string>

#include "../book.h"

class HostWindow : public QMainWindow {
    Q_OBJECT
    
private:
    QLabel* left_label;
    QLabel* right_label;
    QTableWidget* table;
    QTableWidget* book_table;

public:
    HostWindow(const std::vector<Book>& books, QWidget* parent = nullptr);
    virtual ~HostWindow();

    void update_books(const std::vector<Book>& books, std::string state, std::string book_name, std::string client_name, std::string time, bool flag);
};