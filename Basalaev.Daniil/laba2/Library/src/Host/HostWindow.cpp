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
    m_hostKillButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #ff6b6b;"
        "   color: white;"
        "   font-size: 14px;"
        "   font-weight: bold;"
        "   border-radius: 8px;"
        "   padding: 10px;"
        "   border: 1px solid #ff4c4c;"
        "}"
        "QPushButton:hover {"
        "   background-color: #ff4c4c;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #ff3b3b;"
        "}"
    );
    layout->addWidget(m_hostKillButton);

    m_clientList = new QListWidget(this);
    m_clientList->setMinimumHeight(200);
    m_clientList->setMinimumWidth(400);
    m_clientList->setStyleSheet(
        "QListWidget {"
        "   background-color: #f4f4f4;"
        "   border: 1px solid #c0c0c0;"
        "   border-radius: 5px;"
        "   padding: 5px;"
        "   font-size: 14px;"
        "   color: #333333;"
        "}"
        "QListWidget::item {"
        "   padding: 5px;"
        "   border-bottom: 1px solid #d0d0d0;"
        "}"
        "QListWidget::item:selected {"
        "   background-color: #87cefa;"
        "   color: #ffffff;"
        "}"
    );
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
    centralWidget->setStyleSheet(
        "QWidget {"
        "   border: 2px solid #87cefa;"
        "   border-radius: 10px;"
        "   background-color: #ffffff;"
        "}"
    );

    connect(m_hostKillButton, &QPushButton::clicked, this, &HostWindow::terminateHost);
}

HostWindow::~HostWindow() = default;

void HostWindow::updateClientsInfo(std::vector<utils::ClientInfoWithTimer> const& clientsInfo)
{
    m_clientList->clear();
    for (auto const& clientInfo : clientsInfo)
    {
        QString const text = (clientInfo.info.secondsToKill == 0) ? QString("[Client][ID=%1] disconnected").arg(QString::number(clientInfo.info.clientId)) : clientInfo.info.toQString();

        QListWidgetItem* clientItem = new QListWidgetItem(text, m_clientList);

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
    m_letClose = true;
    close();
    QCoreApplication::quit();
}
