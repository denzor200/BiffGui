#include "mainwindow.h"
#include <QApplication>
#include <QtGui>
#include <QMenuBar>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setOrganizationName("Denzor and Che");
    QApplication::setApplicationName("BiffTannen");
    MainWindow w;
    w.show();
    return a.exec();
}
