#include "releaseserver.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ReleaseServer server(2323);
    server.show();
    return a.exec();
}
