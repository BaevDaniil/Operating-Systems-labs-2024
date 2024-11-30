#include "host_window.h"
#include "includes.h"

#include <QApplication>

void HostChatWindow::send_msg()
{

}

void HostChatWindow::read_msg() {

}

void HostChatWindow::setup_conn() {

}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    HostChatWindow w;
    w.show();
    return a.exec();
}
