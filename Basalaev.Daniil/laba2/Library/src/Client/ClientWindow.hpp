#pragma once

#include "Common/Book.hpp"
#include "Common/LibraryWindowImpl.hpp"

#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QListWidget>
#include <QStackedWidget>
#include <QString>
#include <vector>
#include <string>
#include <QWidget>

class ClientWindow : public LibraryWindowImpl
{
    Q_OBJECT

public:
    ClientWindow(alias::id_t, alias::book_container_t const&, QWidget* parent = nullptr);
    ~ClientWindow() override;

    std::string getCurrentBook() const;

    void onSuccessTakeBook(std::string const& bookName, alias::id_t clientId) override;
    void onSuccessReturnBook(std::string const& bookName, alias::id_t clientId) override;

signals:
    void bookSelected(std::string const& bookName, alias::id_t);
    void bookReturned(std::string const& bookName, alias::id_t);

private slots:
    void selectBook();
    void stopReading();
    void terminateClient();

private:
    void createBookView(alias::book_container_t const&);
    void createReadingView();

    // For switch windows (started <-> reading)
    QStackedWidget* stackedWidget; 
    QPushButton* selectButton;

    QLabel* readingLabel;
    QPushButton* cancelReadingButton;
};