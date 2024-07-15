#include <QtGui/QApplication>
#include "xmainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    XMainWindow w;
    w.setFixedSize(971, 804);
    w.setWindowFlags(Qt::FramelessWindowHint);
    w.show();

    return a.exec();
}
