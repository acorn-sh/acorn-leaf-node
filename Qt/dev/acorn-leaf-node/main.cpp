#include "MainWindow.h"
#include "ResourceManager.h"

#include <QApplication>
#include <QFile>
#include <QTextStream>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Load the stylesheet
    QFile file(":/styles/dark.qss"); // Assuming dark.qss is stored in the resources
    if (file.open(QFile::ReadOnly | QFile::Text))
    {
        QTextStream ts(&file);
        a.setStyleSheet(ts.readAll()); // Apply the stylesheet
        file.close();
    }
    else
    {
        qWarning("Could not open the stylesheet file.");
    }

    // Initialize resources and main window
    ResourceManager resourceManager;
    MainWindow w;
    w.show();

    return a.exec();
}
