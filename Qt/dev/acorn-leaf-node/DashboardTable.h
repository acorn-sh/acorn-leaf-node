#ifndef DASHBOARDTABLE_H
#define DASHBOARDTABLE_H

#include <QObject>
#include <QStringList>
#include <QWidget>
#include <QPushButton>
#include <QTableWidget>
#include <QMap>  // Include QMap for the container ID to row index mapping
#include "ResourceManager.h"
#include "ui_MainWindow.h"
#include "Components.h"

class DashboardTable : public QObject
{
    Q_OBJECT

public:
    explicit DashboardTable(Ui::MainWindow* ui, ResourceManager* resourceManager, QObject* parent = nullptr);
    ~DashboardTable();

    QStringList getRunningContainers();
    QStringList getContainerStats();
    QStringList getContainerInfo(const QString container_id);

    void populateDashboardTable(const QStringList& containers, const QStringList& stats, const QStringList& info);
    void replaceButtonsWithProgressBar(int row);
    void executeDockerCommand(int row, const QString& containerId, const QString& command);

signals:
    void startRequested(int row);
    void stopRequested(int row);
    void pauseRequested(int row);
    void restartRequested(int row);
    void deleteRequested(int row);

    void commandOutputReady(const QString& output);
    void dataUpdated();

private:
    void setupTable();

    Ui::MainWindow* ui;
    ResourceManager* resourceManager;
    Components* components;

    QMap<QString, int> containerIdToRowMap;

    void addControlButtons(int row);
};

#endif // DASHBOARDTABLE_H
