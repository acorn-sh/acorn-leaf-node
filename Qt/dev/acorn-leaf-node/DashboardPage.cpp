#include "DashboardPage.h"
#include "ResourceManager.h"
#include "Terminal.h"

#include <QMessageBox>
#include <QProcess>
#include <QtConcurrent/QtConcurrent>

DashboardPage::DashboardPage(Ui::MainWindow* ui, ResourceManager* resourceManager, QObject* parent)
    : QObject(parent), ui(ui), resourceManager(resourceManager), dashboardTable(new DashboardTable(ui, resourceManager, this)), terminal(new Terminal(ui->terminal, ui->expandButton, this))
{
    terminal->setupTerminal();
    setupConnections();
    refreshTable();
}

DashboardPage::~DashboardPage() {
    delete dashboardTable;
    delete terminal;
}

void DashboardPage::setupConnections()
{
    connect(dashboardTable, &DashboardTable::startRequested, this, &DashboardPage::handleStart);
    connect(dashboardTable, &DashboardTable::stopRequested, this, &DashboardPage::handleStop);
    connect(dashboardTable, &DashboardTable::pauseRequested, this, &DashboardPage::handlePause);
    connect(dashboardTable, &DashboardTable::restartRequested, this, &DashboardPage::handleRestart);
    connect(dashboardTable, &DashboardTable::deleteRequested, this, &DashboardPage::handleDelete);

    connect(dashboardTable, &DashboardTable::dataUpdated, this, &DashboardPage::refreshTable);
    connect(ui->refreshDashboardButton, &QPushButton::clicked, this, &DashboardPage::handleRefreshDashboard);
}

void DashboardPage::refreshTable()
{
    QStringList containers = dashboardTable->getRunningContainers();
    QStringList stats = dashboardTable->getContainerStats();

    QStringList info;
    for (const QString& container : containers) {
        QJsonDocument doc = QJsonDocument::fromJson(container.toUtf8());
        QJsonArray jsonArray = doc.array();
        for (const QJsonValue& value : jsonArray) {
            QJsonObject jsonObject = value.toObject();
            QString containerId = jsonObject.value("container_id").toString();
            QStringList containerInfo = dashboardTable->getContainerInfo(containerId);
            info.append(containerInfo.join("\n"));  // Combine into a single QStringList
        }
    }

    QMetaObject::invokeMethod(this, [this, containers, stats, info]() {
        dashboardTable->populateDashboardTable(containers, stats, info);
        terminal->appendLog("Dashboard table refreshed with updated container stats.");
    }, Qt::QueuedConnection);
}

void DashboardPage::handleStart(int row)
{
    QString containerId = ui->dashboardTable->item(row, 1)->text();
    dashboardTable->executeDockerCommand(row, containerId, "start");
    terminal->appendLog("Starting container: " + containerId);
}

void DashboardPage::handleStop(int row)
{
    QString containerId = ui->dashboardTable->item(row, 1)->text();
    dashboardTable->executeDockerCommand(row, containerId, "stop");
    terminal->appendLog("Stopping container: " + containerId);
}

void DashboardPage::handlePause(int row)
{
    QString containerId = ui->dashboardTable->item(row, 1)->text();
    dashboardTable->executeDockerCommand(row, containerId, "pause");
    terminal->appendLog("Pausing container: " + containerId);
}

void DashboardPage::handleRestart(int row)
{
    QString containerId = ui->dashboardTable->item(row, 1)->text();
    dashboardTable->executeDockerCommand(row, containerId, "restart");
    terminal->appendLog("Restarting container: " + containerId);
}

void DashboardPage::handleDelete(int row)
{
    QString containerId = ui->dashboardTable->item(row, 1)->text();
    dashboardTable->executeDockerCommand(row, containerId, "rm");
    terminal->appendLog("Deleting container: " + containerId);
}

void DashboardPage::handleRefreshDashboard()
{
    refreshTable();  // This will refresh the dashboard table
}
