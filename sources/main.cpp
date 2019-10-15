#include "mainwindow.h"
#include <QApplication>
#include <QtGui>
#include <QMenuBar>

int main(int argc, char *argv[])
{
    // Creating QApplication and setting params..
    QApplication app(argc, argv);
    QApplication::setOrganizationName("Denzor and Che");
    QApplication::setApplicationName("BiffTannen");

    // Parsing command line..
    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addOptions({
        {{"c", "current"},
            QCoreApplication::translate("main", "Change the current working directory."),
            QCoreApplication::translate("main", "directory")},
    });
    parser.process(app);

    // Initial commands handling..
    QString targetDir = parser.value("current");
    if (targetDir != "")
    {
        if (!QDir::setCurrent(targetDir))
                qDebug() << "Could not change the current working directory";
    }

    // Creating MainWindow and executing..
    MainWindow w;
    w.show();
    return app.exec();
}
