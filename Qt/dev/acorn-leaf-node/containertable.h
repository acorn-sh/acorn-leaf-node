#ifndef CONTAINERTABLE_H
#define CONTAINERTABLE_H

#include <QObject>
#include <QProcess>
#include <QMap>
#include "ui_mainwindow.h"

class ContainerTable : public QObject
{
    Q_OBJECT

public:
    ContainerTable(Ui::MainWindow *ui);
    void setupTable();
    void updateRuntime();
    void displayLogInTerminal(const QString &log);
    void handleDockerCommand(int row, const QString &scriptName);

private:
    Ui::MainWindow *ui;
    QMap<int, QProcess*> activeProcesses;

    void populateContainerData();
    void addControlButtons(int row, const QString &activeStatus);
    void adjustColumnWidths();
    void handleStart(int row);
    void handleStop(int row);
    void handlePause(int row);
    void handleRestart(int row);

};

#endif // CONTAINERTABLE_H
