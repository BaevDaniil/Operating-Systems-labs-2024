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

public:
    HostWindow(const std::vector<Book>& books, QWidget* parent = nullptr);
    virtual ~HostWindow();

    void update_books(const std::vector<Book>& books);

private:
    QLabel* left_label;
    QListWidget* book_qlist;
};