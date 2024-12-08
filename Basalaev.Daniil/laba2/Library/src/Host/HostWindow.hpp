#pragma once

#include "Common/Book.hpp"
#include "Common/LibraryWindowImpl.hpp"
#include "Common/Utils.hpp"

#include <QLabel>
#include <QPushButton>
#include <vector>
#include <string>

class HostWindow : public LibraryWindowImpl
{
    Q_OBJECT

public:
    HostWindow(std::string const& hostTitle, alias::book_container_t const& books, QWidget* parent = nullptr);
    ~HostWindow() override;

    void updateClientsInfo(std::vector<utils::ClientInfoWithTimer> const&);
    void notifyClientTerminated(alias::id_t);

signals:
    void resetSignalTimer();
    void stopSignalTimer();

private slots:
    void terminateHost();

private:
    QListWidget* m_clientList;
    QLabel* m_hostTitle;
    QPushButton* m_hostKillButton;
};
