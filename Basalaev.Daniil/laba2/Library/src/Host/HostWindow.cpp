#include "HostWindow.hpp"
#include "Common/Logger.hpp"

#include <QMessageBox>
#include <QApplication>
#include <QDockWidget>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QString>
#include <QPalette>
#include <QFont>

HostWindow::HostWindow(std::string const& hostTitle, alias::book_container_t const& books, QWidget* parent)
    : LibraryWindowImpl(alias::HOST_ID, books, parent)
{
    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(centralWidget);

    m_hostTitle = new QLabel(QString::fromStdString(hostTitle), this);
    QFont titleFont = m_hostTitle->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    m_hostTitle->setFont(titleFont);
    m_hostTitle->setAlignment(Qt::AlignCenter);
    layout->addWidget(m_hostTitle);

    layout->addWidget(m_bookList);

    m_hostKillButton = new QPushButton("Terminate Host", this);
    layout->addWidget(m_hostKillButton);

    m_clientList = new QListWidget(this);
    m_clientList->setMinimumHeight(200);
    m_clientList->setMinimumWidth(400);
    QDockWidget* clientDock = new QDockWidget("Clients", this);
    clientDock->setWidget(m_clientList);
    addDockWidget(Qt::RightDockWidgetArea, clientDock);

    setCentralWidget(centralWidget);
    setWindowTitle("Host Window");
    resize(800, 600);

    QPalette palette = centralWidget->palette();
    palette.setColor(QPalette::Window, QColor(240, 240, 240));
    centralWidget->setAutoFillBackground(true);
    centralWidget->setPalette(palette);

    connect(m_hostKillButton, &QPushButton::clicked, this, &HostWindow::terminateHost);
}

HostWindow::~HostWindow() = default;

void HostWindow::updateClientsInfo(std::vector<utils::ClientInfoWithTimer> const& clientsInfo)
{
    m_clientList->clear();
    for (auto const& clientInfo : clientsInfo)
    {
        QListWidgetItem* clientItem = new QListWidgetItem(clientInfo.info.toQString(), m_clientList);

        if (clientInfo.info.readingBook.empty())
        {
            clientItem->setForeground(QColor(Qt::red));
        }
        else
        {
            clientItem->setForeground(QColor(Qt::green));
        }

        m_clientList->addItem(clientItem);
    }
}

void HostWindow::notifyClientTerminated(alias::id_t id)
{
    std::string const msg = "Client[ID=" + std::to_string(id) + "] terminated.";
    QMessageBox::information(this, "Terminate Client", QString::fromStdString(msg));
    LOG_INFO(HOST_LOG, msg);
}

void HostWindow::terminateHost()
{
    QMessageBox::information(this, "Terminate Host", "Host terminated.");
    LOG_INFO(HOST_LOG, "Terminate Host");
    // kill(clientPid, SIGKILL); // and kill client too
    std::exit(0);
}
