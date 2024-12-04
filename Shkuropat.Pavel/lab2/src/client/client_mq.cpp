#include "client_gui.hpp"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    ClientGUI client;
    client.show();
    return app.exec();
}
