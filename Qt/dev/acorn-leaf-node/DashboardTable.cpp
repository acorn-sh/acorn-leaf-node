#include "DashboardTable.h"
#include <QProcess>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

DashboardTable::DashboardTable(Ui::MainWindow* ui, ResourceManager* resourceManager, QObject* parent)
    : QObject(parent), ui(ui), resourceManager(resourceManager)
{
    components = new Components(resourceManager);

    QStringList headers = {"Image ID", "Container ID", "Status", "Time", "CPU", "GPU", "MEM", "NET", "PORT", "Rate", "Total"};
    ui->dashboardTable->setColumnCount(headers.size());
    ui->dashboardTable->setHorizontalHeaderLabels(headers);
    ui->dashboardTable->setStyleSheet("background-color: #d5d3e0; color: black;");

    ui->dashboardTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->dashboardTable->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->dashboardTable->setSelectionBehavior(QAbstractItemView::SelectItems);
    ui->dashboardTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    for (int i = 0; i < headers.size(); ++i) {
        ui->dashboardTable->horizontalHeader()->setSectionResizeMode(i, QHeaderView::ResizeToContents);
    }

    ui->dashboardTable->horizontalHeader()->setMinimumSectionSize(60);
}

DashboardTable::~DashboardTable() {
    delete components;
}

QStringList DashboardTable::getRunningContainers()
{
    QString pythonPath = resourceManager->getPythonPath();
    QString scriptPath = resourceManager->getScriptPath("list_docker_containers.py");

    QProcess process;
    process.setProcessEnvironment(resourceManager->getProcessEnvironment());
    process.start(pythonPath, QStringList() << scriptPath << "-c" << "get_running_containers");
    process.waitForFinished();

    QString output = process.readAll().trimmed();
    return output.split("\n", Qt::SkipEmptyParts);
}

QStringList DashboardTable::getContainerStats()
{
    QString pythonPath = resourceManager->getPythonPath();
    QString scriptPath = resourceManager->getScriptPath("list_docker_containers.py");

    QProcess process;
    process.setProcessEnvironment(resourceManager->getProcessEnvironment());
    process.start(pythonPath, QStringList() << scriptPath << "-c" << "get_container_stats");
    process.waitForFinished();

    QString output = process.readAll().trimmed();
    return output.split("\n", Qt::SkipEmptyParts);
}

void DashboardTable::populateDashboardTable(const QStringList& containers, const QStringList& stats)
{
    ui->dashboardTable->setRowCount(0);  // Clear the table
    containerIdToRowMap.clear();  // Clear the container ID to row mapping

    // First loop: populate the table with container data and build the containerIdToRowMap
    for (const QString& container : containers) {
        QJsonDocument doc = QJsonDocument::fromJson(container.toUtf8());
        QJsonArray jsonArray = doc.array();
        for (const QJsonValue& value : jsonArray) {
            QJsonObject jsonObject = value.toObject();
            QStringList rowData;
            QString containerId = jsonObject.value("container_id").toString();

            rowData << jsonObject.value("image").toString()
                    << containerId
                    << "" // Placeholder for Status Buttons
                    << jsonObject.value("created").toString()
                    << ""  // Placeholder for CPU
                    << ""  // Placeholder for GPU
                    << ""  // Placeholder for MEM
                    << ""  // Placeholder for NET
                    << jsonObject.value("ports").toString()
                    << ""  // Placeholder for Rate
                    << ""; // Placeholder for Total

            int row = ui->dashboardTable->rowCount();
            ui->dashboardTable->insertRow(row);
            for (int i = 0; i < rowData.size(); ++i) {
                ui->dashboardTable->setItem(row, i, new QTableWidgetItem(rowData[i].trimmed()));
            }

            // Store the mapping of container ID to row index
            containerIdToRowMap[containerId] = row;

            addControlButtons(row);
        }
    }

    // Second loop: update the existing rows with the statistics
    for (const QString& stat : stats) {
        QJsonDocument doc = QJsonDocument::fromJson(stat.toUtf8());
        QJsonArray jsonArray = doc.array();
        for (const QJsonValue& value : jsonArray) {
            QJsonObject jsonObject = value.toObject();
            QString containerId = jsonObject.value("container_id").toString();

            // Use the containerIdToRowMap to find the corresponding row
            int row = containerIdToRowMap.value(containerId, -1);
            if (row != -1) {
                ui->dashboardTable->setItem(row, 4, new QTableWidgetItem(jsonObject.value("cpu").toString()));
                ui->dashboardTable->setItem(row, 5, new QTableWidgetItem(jsonObject.value("gpu").toString()));
                ui->dashboardTable->setItem(row, 6, new QTableWidgetItem(jsonObject.value("mem_usage").toString() + " / " + jsonObject.value("mem_limit").toString()));
                ui->dashboardTable->setItem(row, 7, new QTableWidgetItem(jsonObject.value("net_io").toString()));
            }
        }
    }
}

void DashboardTable::addControlButtons(int row)
{
    QWidget* controlWidget = new QWidget();
    QHBoxLayout* controlLayout = new QHBoxLayout(controlWidget);
    controlLayout->setContentsMargins(0, 0, 0, 0);
    controlLayout->setAlignment(Qt::AlignCenter);
    controlLayout->setSpacing(5);

    QPushButton* btnStart = components->createButtonWithIcon("start");
    QPushButton* btnStop = components->createButtonWithIcon("stop");
    QPushButton* btnPause = components->createButtonWithIcon("pause");
    QPushButton* btnRestart = components->createButtonWithIcon("restart");

    controlLayout->addWidget(btnStart);
    controlLayout->addWidget(btnStop);
    controlLayout->addWidget(btnPause);
    controlLayout->addWidget(btnRestart);

    controlWidget->setLayout(controlLayout);
    ui->dashboardTable->setCellWidget(row, 2, controlWidget);

    connect(btnStart, &QPushButton::clicked, this, [this, row]() {
        emit startRequested(row);
        replaceButtonsWithProgressBar(row);
    });
    connect(btnStop, &QPushButton::clicked, this, [this, row]() {
        emit stopRequested(row);
        replaceButtonsWithProgressBar(row);
    });
    connect(btnPause, &QPushButton::clicked, this, [this, row]() {
        emit pauseRequested(row);
        replaceButtonsWithProgressBar(row);
    });
    connect(btnRestart, &QPushButton::clicked, this, [this, row]() {
        emit restartRequested(row);
        replaceButtonsWithProgressBar(row);
    });
}

void DashboardTable::replaceButtonsWithProgressBar(int row)
{
    QWidget* progressWidget = components->createProgressBarWidget(1, ui->dashboardTable);
    ui->dashboardTable->setCellWidget(row, 2, progressWidget);
}

void DashboardTable::executeDockerCommand(int row, const QString& containerId, const QString& command)
{
    replaceButtonsWithProgressBar(row);

    QString pythonPath = resourceManager->getPythonPath();
    QString scriptPath = resourceManager->getScriptPath("docker_commands.py");

    QProcess* process = new QProcess(this);
    process->setProcessEnvironment(resourceManager->getProcessEnvironment());
    process->start(pythonPath, QStringList() << scriptPath << containerId << command);

    connect(process, &QProcess::readyRead, this, [this, process]() {
        QString output = process->readAll().trimmed();
        emit commandOutputReady(output);
        qDebug() << "Command Output:" << output;
    });

    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, [this, row]() {
        addControlButtons(row);
        emit dataUpdated();
    });
}
