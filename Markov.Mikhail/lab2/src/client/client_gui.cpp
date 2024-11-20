#include "client_gui.hpp"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    ClientMainWindow window;
    window.show();

    return app.exec();
}