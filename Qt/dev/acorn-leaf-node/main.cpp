#include <QApplication>
#include <QFile>
#include <QTextStream>
#include "mainwindow.h"
#include "ResourceManager.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QFile file(":/styles/dark.qss");
    if (file.open(QFile::ReadOnly | QFile::Text))
    {
        QTextStream ts(&file);
        qApp->setStyleSheet(ts.readAll());
    }

    ResourceManager resourceManager;
    MainWindow w;
    w.show();
    return a.exec();
}
