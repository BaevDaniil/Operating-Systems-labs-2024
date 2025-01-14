#ifndef GUI_HOST_H
#define GUI_HOST_H

#include <string>
#include <QMainWindow>
#include <QtGui>
#include <QTableView>
#include <QApplication>
#include <QTimer>
#include <memory>
#include "../utilsConfigurations/gameWorld.h"

namespace Ui 
{
    class guiHostWindow;
};

class guiHost: public QMainWindow
{
Q_OBJECT
public:
    guiHost(std::shared_ptr<GameWorld> gameWorld, int hostPid);

    ~guiHost();

signals:
    void countGoatsSend(int number);
    void wolfNumberSend(int number);
private slots:
    void countGoatsEnter();
    void wolfNumberEnter();
    void viewRoundTable();
public slots:
    void gameover();
    void writeLog(const std::string &log_string);
    void countGoatsRSend(int number);
    void setOpenGame();

private:
    Ui::guiHostWindow *uiHost;

    QStandardItemModel *modelRowTable;

    QTimer *wolfNumberTimer;

    std::shared_ptr<GameWorld> gameW;

    QString goatStatusToQStr(goatStatus statGoat);
    QString connectStateTOQStr(connectState state);

    guiHost(const guiHost&) = delete;
    guiHost& operator=(const guiHost&) = delete;

    int lastAliveNumber;
    int isGameOver=false;

};


#endif