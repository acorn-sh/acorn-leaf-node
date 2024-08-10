#ifndef CONTAINERTABLE_H
#define CONTAINERTABLE_H

#include <QObject>
#include "ui_mainwindow.h"

class ContainerTable : public QObject
{
    Q_OBJECT

public:
    ContainerTable(Ui::MainWindow *ui);
    void updateRuntime();

private:
    Ui::MainWindow *ui;

    void setupTable();
    void populateDummyData();
    void addControlButtons(int row, const QString &activeStatus);
    void adjustColumnWidths();
    void handlePlay(int row);
    void handleStop(int row);
    void handlePause(int row);
    void handleInstall(int row);
};

#endif // CONTAINERTABLE_H
