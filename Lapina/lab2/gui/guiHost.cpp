
#include <unistd.h>
#include <QList>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QString>
#include <QStringList>
#include <QDateTime>
#include <QPalette>

#include <sys/syslog.h>
#include <mutex>

#include "guiHost.h"
#include "ui_guiHostWindow.h"

guiHost::guiHost(std::shared_ptr<GameWorld> gameWorld, int hostPid): QMainWindow((QWidget*)0),uiHost(new Ui::guiHostWindow),gameW(gameWorld)
{
    uiHost->setupUi(this);
    uiHost->hostPidSet->setText(QString::number(hostPid));
    uiHost->wolfNumber->setValidator(new QIntValidator(1, 100, this));

    modelRowTable = new QStandardItemModel(uiHost->tableRound);

    QObject::connect(uiHost->numGoatsEnter, SIGNAL(returnPressed()), this, SLOT(countGoatsEnter()));
    QObject::connect(uiHost->wolfNumber, SIGNAL(returnPressed()), this, SLOT(wolfNumberEnter()));
    wolfNumberTimer = new QTimer(this);
    QObject::connect(wolfNumberTimer, SIGNAL(timeout()), this, SLOT(viewRoundTable()));
    wolfNumberTimer->start(360);

    auto palette = uiHost->validWolfNumber->palette();
    palette.setColor(palette.WindowText, Qt::blue);
    palette.setColor(palette.Light, QColor(0, 0, 255));
    palette.setColor(palette.Dark, QColor(0, 0, 0));   
    uiHost->validWolfNumber->setPalette(palette);

    auto palette2 = uiHost->AliveGoat->palette();
    palette2.setColor(palette2.WindowText, Qt::blue);
    palette2.setColor(palette2.Light, QColor(0, 0, 255));
    palette2.setColor(palette2.Dark, QColor(0, 0, 0));   
    uiHost->AliveGoat->setPalette(palette2);

    auto palette3 = uiHost->DeadGoat->palette();
    palette3.setColor(palette3.WindowText, Qt::blue);
    palette3.setColor(palette3.Light, QColor(0, 0, 255));
    palette3.setColor(palette3.Dark, QColor(0, 0, 0));   
    uiHost->DeadGoat->setPalette(palette3);

    auto palette4 = uiHost->CaughtGoat->palette();
    palette4.setColor(palette4.WindowText, Qt::blue);
    palette4.setColor(palette4.Light, QColor(0, 0, 255));
    palette4.setColor(palette4.Dark, QColor(0, 0, 0));   
    uiHost->CaughtGoat->setPalette(palette4);
}

void guiHost::countGoatsEnter() 
{
    bool ok;
    int num = uiHost->numGoatsEnter->text().toInt(&ok, 10);
    if (ok) {
        emit countGoatsSend(num);
        uiHost->numGoatsEnter->setReadOnly(true);
    }
}

void guiHost::wolfNumberEnter() {
    bool ok;
    int num = uiHost->wolfNumber->text().toInt(&ok, 10);
    if (ok) {
        emit wolfNumberSend(num);
        uiHost->wolfNumber->clear();
        uiHost->yourWolfNumber->setText(QString::number(num));
    }
}

void guiHost::gameover()
{
    isGameOver = true;
    uiHost->wolfNumber->setReadOnly(true);
    uiHost->yourWolfNumber->clear();
    uiHost->gameOver->setEnabled(true);
    uiHost->gameOver->setText(QString("Игра окончена"));
    uiHost->gameOver->setStyleSheet("QLineEdit"
                            "{"
                            "background : LightBlue;"
                            "}");
}

void guiHost::countGoatsRSend(int number)
{
    uiHost->remainsGoatEdit->setText(QString::number(number));
    if (number==0)
    {
        setOpenGame();
    }
}

void guiHost::writeLog(const std::string &log_string)
{
    QDateTime datetime = QDateTime::currentDateTime();
    uiHost->LogEdit->insertPlainText(QString("[") + datetime.toString() + QString::fromStdString("]:  " +log_string + "\n"));
}

void guiHost::setOpenGame()
{
    uiHost->tabWidgetGame->setTabEnabled(1,true);
    uiHost->tabWidgetGame->setCurrentIndex(1);
    uiHost->tabWidgetGame->setTabEnabled(2,true);

    lastAliveNumber = gameW->goatMap.len();
    uiHost->tableRound->setEditTriggers(QAbstractItemView::NoEditTriggers);
    
    QStringList columnName = QString("Pid goat, GoatNum, GoatStatus, Connection").simplified().split(",");
    
    modelRowTable->setHorizontalHeaderLabels(columnName);
    uiHost->tableRound->setModel(modelRowTable);
    uiHost->tableRound->setColumnWidth(0, uiHost->tableRound->width() * 0.2);
    uiHost->tableRound->setColumnWidth(1, uiHost->tableRound->width() * 0.2);
    uiHost->tableRound->setColumnWidth(2, uiHost->tableRound->width() * 0.3);
    uiHost->tableRound->setColumnWidth(3, uiHost->tableRound->width() * 0.3);
}

void guiHost::viewRoundTable()
{
    if (isGameOver)
    {
        return;
    }

    uiHost->validWolfNumber->display(gameW->wolfNumber.load());

    int currentAlive = gameW->aliveGoatNumber.load();
    uiHost->AliveGoat->display(currentAlive);
    uiHost->DeadGoat->display(gameW->deadGoatNumder.load());

    if (lastAliveNumber>currentAlive)
    {
        uiHost->CaughtGoat->display(lastAliveNumber-currentAlive);
    }
    else
    {
        uiHost->CaughtGoat->display(0);
    }
    lastAliveNumber=currentAlive;
    
    if(gameW->time.load() >= 0){
        uiHost->Timer->display(gameW->time.load());
    }

    modelRowTable->removeRows(0, modelRowTable->rowCount());

    std::vector<pid_t> keysGoat = gameW->goatMap.getAllKeys();  

    for (size_t i = 0; i < keysGoat.size(); i++)
    {
        QList<QStandardItem *> list_;
        list_.append(new QStandardItem(QString::number(keysGoat[i])));

        list_.append(new QStandardItem(QString::number(gameW->goatMap.get(keysGoat[i])->goatNumber.load())));
    
        list_.append(new QStandardItem(goatStatusToQStr(gameW->goatMap.get(keysGoat[i])->status.load())));

        list_.append(new QStandardItem(connectStateTOQStr(gameW->goatMap.get(keysGoat[i])->stateClient.load())));

        modelRowTable->appendRow(list_);
    }

}

QString guiHost::goatStatusToQStr(goatStatus statGoat)
{
    if (statGoat==goatStatus::ALIVE)
    {
        return QString("живой");
    }
    else{
        return QString("мёртвый");
    }
}

QString guiHost::connectStateTOQStr(connectState state)
{
    if (state==connectState::CONNECTION)
    {
        return QString("соединён");
    }
    else{
        return QString("отсоединён");
    }
}

guiHost::~guiHost() {
    delete uiHost;
    delete wolfNumberTimer;
}