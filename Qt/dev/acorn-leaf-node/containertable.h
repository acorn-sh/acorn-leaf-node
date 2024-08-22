#ifndef CONTAINERTABLE_H
#define CONTAINERTABLE_H

#include <QObject>
#include <QProcess>
#include <QMap>
#include <memory>
#include "ui_mainwindow.h"
#include "ResourceManager.h"

class ContainerTable : public QObject
{
    Q_OBJECT

public:
    explicit ContainerTable(Ui::MainWindow *ui);
    ~ContainerTable() override; // Explicitly declare the destructor

    void setupTable();
    void updateRuntime();
    void displayLogInTerminal(const QString &log);
    void handleDockerCommand(int row, const QString &scriptName);

private:
    Ui::MainWindow *ui;
    QMap<int, QProcess*> activeProcesses;
    QMap<int, QTimer*> activeTimers;  // Add this line to manage the timers.

    std::unique_ptr<ResourceManager> resourceManager; // Use unique_ptr for automatic memory management

    void populateContainerData();
    void updateRow(int row, const QStringList &details);
    void addRow(const QStringList &details);
    void addControlButtons(int row, const QString &activeStatus);
    void adjustColumnWidths();
    void handleStart(int row);
    void handleStop(int row);
    void handlePause(int row);
    void handleRestart(int row);
    void startElapsedTimer(int row, const QString &initialElapsedTime, bool isRunning);
    void simulatePayment(int row);
    int findRowByContainerId(const QString &containerId) const;

};

#endif // CONTAINERTABLE_H
