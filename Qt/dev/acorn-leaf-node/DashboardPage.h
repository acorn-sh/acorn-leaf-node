#ifndef DASHBOARDPAGE_H
#define DASHBOARDPAGE_H

#include <QObject>
#include "DashboardTable.h"
#include "ResourceManager.h"
#include "Terminal.h"

#include "ui_MainWindow.h"

class DashboardPage : public QObject
{
    Q_OBJECT

public:
    explicit DashboardPage(Ui::MainWindow* ui, ResourceManager* resourceManager, QObject* parent = nullptr);
    ~DashboardPage();
    void handleRefreshDashboard();

private slots:
    void handleStart(int row);
    void handleStop(int row);
    void handlePause(int row);
    void handleRestart(int row);
    void handleDelete(int row);

    // New slot
    void refreshTable();


private:
    void setupConnections();

    Ui::MainWindow* ui;
    ResourceManager* resourceManager;
    Components* components;
    DashboardTable* dashboardTable;
    Terminal* terminal;
};

#endif // DASHBOARDPAGE_H
