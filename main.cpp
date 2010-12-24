#include <QtGui/QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setApplicationName("Ministrel");
    QCoreApplication::setOrganizationName("FSF");
    MainWindow w;
    w.show();

    return a.exec();
}
