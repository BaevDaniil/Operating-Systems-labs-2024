#include "host_gui.hpp"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    HostGUI host;
    host.show();
    return app.exec();
}
