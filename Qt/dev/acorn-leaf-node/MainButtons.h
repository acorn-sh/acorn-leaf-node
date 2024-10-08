#ifndef MAINBUTTONS_H
#define MAINBUTTONS_H

#include <QObject>
#include "ui_MainWindow.h"

class MainButtons : public QObject
{
    Q_OBJECT

public:
    explicit MainButtons(Ui::MainWindow *ui, QObject *parent = nullptr);
    void setupConnections(QObject *mainWindow);

private:
    Ui::MainWindow *ui;
};

#endif // MAINBUTTONS_H
