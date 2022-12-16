#include "releaseclient.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ReleaseClient client("192.168.0.13", 2323);
    client.show();
    return a.exec();
}
