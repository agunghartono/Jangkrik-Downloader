#include "widget.h"

#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.setWindowFlag(Qt::WindowStaysOnTopHint);
    w.setWindowTitle("Jangkrik Downloader");
    w.show();
    return a.exec();
}
