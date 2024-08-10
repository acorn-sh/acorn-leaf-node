#include <QApplication>
#include <QFile>
#include <QTextStream>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Load styles
    QFile file(":/styles/dark.qss");
    if (file.open(QFile::ReadOnly | QFile::Text))
    {
        QTextStream ts(&file);
        qApp->setStyleSheet(ts.readAll());
    }

    MainWindow w;
    w.show();
    return a.exec();
}
