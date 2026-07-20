#include "Arona.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    Arona window;
    window.show();
    return app.exec();
}
