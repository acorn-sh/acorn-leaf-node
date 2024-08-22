#include "containertable.h"
#include <QTime>
#include <QProcess>
#include <QDebug>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QPushButton>
#include <QRegularExpression>
#include <QScrollBar>
#include <QProgressBar>
#include <QTimer>

ContainerTable::ContainerTable(Ui::MainWindow *ui)
    : ui(ui), resourceManager(std::make_unique<ResourceManager>()) // Initialize the unique_ptr
{
    setupTable();
    populateContainerData();
}

ContainerTable::~ContainerTable() {
    // No need to manually delete resourceManager, unique_ptr handles it automatically
}

void ContainerTable::setupTable()
{
    QStringList headers = {"Container Name", "Image ID", "Container ID", "Status", "Time", "CPU", "GPU", "MEM", "NET", "PORT"};
    ui->containerTable->setColumnCount(headers.size());
    ui->containerTable->setHorizontalHeaderLabels(headers);
    ui->containerTable->setStyleSheet("background-color: #d5d3e0; color: black;");

    ui->containerTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->containerTable->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->containerTable->setSelectionBehavior(QAbstractItemView::SelectItems);
    ui->containerTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    for (int i = 0; i < headers.size(); ++i) {
        ui->containerTable->horizontalHeader()->setSectionResizeMode(i, QHeaderView::ResizeToContents);
    }

    ui->containerTable->horizontalHeader()->setMinimumSectionSize(70);

    adjustColumnWidths();
}

void ContainerTable::populateContainerData()
{
    QStringList sudoacornData;

    QProcessEnvironment env = resourceManager->getProcessEnvironment();
    QString pythonPath = resourceManager->getPythonPath();
    QString scriptPath = resourceManager->getScriptPath("list_docker_image.py");

    QProcess *process = new QProcess(this);
    process->setProcessEnvironment(env);
    process->setProcessChannelMode(QProcess::MergedChannels);

    connect(process, &QProcess::readyRead, [process, &sudoacornData, this]() {
        QString output = process->readAll();
        displayLogInTerminal(output);
        sudoacornData = output.split("\n", Qt::SkipEmptyParts);
    });

    process->start(pythonPath, QStringList() << scriptPath);
    process->waitForFinished();

    ui->containerTable->setRowCount(0);  // Clear table before repopulating

    for (int i = 0; i < sudoacornData.size(); ++i) {
        QStringList details = sudoacornData[i].split("|", Qt::SkipEmptyParts);

        if (details.size() < 9) continue;

        int existingRow = findRowByContainerId(details[2].trimmed());

        if (existingRow != -1) {
            updateRow(existingRow, details);
        } else {
            addRow(details);
        }
    }

    adjustColumnWidths();
}

int ContainerTable::findRowByContainerId(const QString &containerId) const
{
    for (int i = 0; i < ui->containerTable->rowCount(); ++i) {
        if (ui->containerTable->item(i, 2)->text() == containerId) {
            return i;
        }
    }
    return -1; // Return -1 if no matching row is found
}

void ContainerTable::addRow(const QStringList &details)
{
    int row = ui->containerTable->rowCount();
    ui->containerTable->insertRow(row);

    QString repository = details[0].trimmed();
    QString imageId = details[1].trimmed();
    QString containerId = details[2].trimmed();
    QString status = details[3].trimmed();
    QString elapsedTime = details[4].trimmed();
    QString cpu = details[5].trimmed();
    QString gpu = "";
    QString mem_usage = details[6].trimmed();
    QString net_io = details[7].trimmed();
    QString port = details[8].trimmed();

    ui->containerTable->setItem(row, 0, new QTableWidgetItem(repository));
    ui->containerTable->setItem(row, 1, new QTableWidgetItem(imageId));
    ui->containerTable->setItem(row, 2, new QTableWidgetItem(containerId));
    ui->containerTable->setItem(row, 3, new QTableWidgetItem(status));
    ui->containerTable->setItem(row, 4, new QTableWidgetItem(elapsedTime));
    ui->containerTable->setItem(row, 5, new QTableWidgetItem(cpu));
    ui->containerTable->setItem(row, 6, new QTableWidgetItem(gpu));
    ui->containerTable->setItem(row, 7, new QTableWidgetItem(mem_usage));
    ui->containerTable->setItem(row, 8, new QTableWidgetItem(net_io));
    ui->containerTable->setItem(row, 9, new QTableWidgetItem(port));

    startElapsedTimer(row, elapsedTime, status == "running");

    addControlButtons(row, status);
}

void ContainerTable::updateRow(int row, const QStringList &details)
{
    QString repository = details[0].trimmed();
    QString imageId = details[1].trimmed();
    QString containerId = details[2].trimmed();
    QString status = details[3].trimmed();
    QString elapsedTime = details[4].trimmed();
    QString cpu = details[5].trimmed();
    QString gpu = "";
    QString mem_usage = details[6].trimmed();
    QString net_io = details[7].trimmed();
    QString port = details[8].trimmed();

    ui->containerTable->setItem(row, 0, new QTableWidgetItem(repository));
    ui->containerTable->setItem(row, 1, new QTableWidgetItem(imageId));
    ui->containerTable->setItem(row, 2, new QTableWidgetItem(containerId));
    ui->containerTable->setItem(row, 3, new QTableWidgetItem(status));
    ui->containerTable->setItem(row, 4, new QTableWidgetItem(elapsedTime));
    ui->containerTable->setItem(row, 5, new QTableWidgetItem(cpu));
    ui->containerTable->setItem(row, 6, new QTableWidgetItem(gpu));
    ui->containerTable->setItem(row, 7, new QTableWidgetItem(mem_usage));
    ui->containerTable->setItem(row, 8, new QTableWidgetItem(net_io));
    ui->containerTable->setItem(row, 9, new QTableWidgetItem(port));

    startElapsedTimer(row, elapsedTime, status == "running");
}

void ContainerTable::startElapsedTimer(int row, const QString &initialElapsedTime, bool isRunning)
{
    QTime initialTime = QTime::fromString(initialElapsedTime, "hh:mm:ss");
    if (!initialTime.isValid() || !isRunning) {
        qDebug() << "Invalid initial time format or container is not running: " << initialElapsedTime;
        ui->containerTable->item(row, 4)->setText("00:00:00");
        return;
    }

    QTimer *timer = new QTimer(this);

    connect(timer, &QTimer::timeout, this, [this, row, initialTime]() mutable {
        initialTime = initialTime.addSecs(1);
        ui->containerTable->item(row, 4)->setText(initialTime.toString("hh:mm:ss"));

        if (initialTime.second() % 60 == 0) {
            simulatePayment(row);
        }
    });

    ui->containerTable->item(row, 4)->setText(initialTime.toString("hh:mm:ss"));
    timer->start(1000);  // Update every second

    activeTimers[row] = timer;  // If using QMap or other storage for timers
}

void ContainerTable::simulatePayment(int row)
{
    QString containerName = ui->containerTable->item(row, 0)->text();
    QString paymentMessage = QString("Simulating payment of 1 dollar for container: %1").arg(containerName);
    displayLogInTerminal(paymentMessage);  // Print to the terminal in the UI
}

void ContainerTable::addControlButtons(int row, const QString &activeStatus)
{
    QWidget* pWidget = new QWidget();
    QHBoxLayout* pButtonLayout = new QHBoxLayout(pWidget);
    pButtonLayout->setAlignment(Qt::AlignCenter);
    pButtonLayout->setContentsMargins(0, 0, 0, 0);
    pButtonLayout->setSpacing(5);

    QPushButton* btnStart = new QPushButton();
    QPushButton* btnStop = new QPushButton();
    QPushButton* btnPause = new QPushButton();
    QPushButton* btnRestart = new QPushButton();

    btnStart->setIcon(resourceManager->getIcon("play"));
    btnStop->setIcon(resourceManager->getIcon("stop"));
    btnPause->setIcon(resourceManager->getIcon("pause"));
    btnRestart->setIcon(resourceManager->getIcon("restart"));

    btnStart->setFixedSize(24, 24);
    btnStop->setFixedSize(24, 24);
    btnPause->setFixedSize(24, 24);
    btnRestart->setFixedSize(24, 24);

    QString buttonStyle = "QPushButton { background-color: #938ea4; }"
                          "QPushButton:hover { background-color: #fbdea3; }"
                          "QPushButton:pressed { background-color: #6d6781; }";

    btnStart->setStyleSheet(buttonStyle);
    btnStop->setStyleSheet(buttonStyle);
    btnPause->setStyleSheet(buttonStyle);
    btnRestart->setStyleSheet(buttonStyle);

    connect(btnStart, &QPushButton::clicked, this, [this, row](){ handleDockerCommand(row, "start"); });
    connect(btnStop, &QPushButton::clicked, this, [this, row](){ handleDockerCommand(row, "stop"); });
    connect(btnPause, &QPushButton::clicked, this, [this, row](){ handleDockerCommand(row, "pause"); });
    connect(btnRestart, &QPushButton::clicked, this, [this, row](){ handleDockerCommand(row, "restart"); });

    pButtonLayout->addWidget(btnStart);
    pButtonLayout->addWidget(btnStop);
    pButtonLayout->addWidget(btnPause);
    pButtonLayout->addWidget(btnRestart);

    pWidget->setLayout(pButtonLayout);
    ui->containerTable->setCellWidget(row, 3, pWidget);
}

void ContainerTable::handleDockerCommand(int row, const QString &command)
{
    QString imageName = ui->containerTable->item(row, 0)->text();
    qDebug() << "Attempting to run command" << command << "for container" << imageName;

    QProcessEnvironment env = resourceManager->getProcessEnvironment();
    QString pythonPath = resourceManager->getPythonPath();
    QString scriptPath = resourceManager->getScriptPath("start_docker_image.py");

    QWidget* statusWidget = new QWidget();
    QHBoxLayout* statusLayout = new QHBoxLayout(statusWidget);
    statusLayout->setContentsMargins(0, 0, 0, 0);
    statusLayout->setAlignment(Qt::AlignCenter);

    QProgressBar* progressBar = new QProgressBar();
    progressBar->setRange(0, 0);
    progressBar->setFixedHeight(1);
    progressBar->setTextVisible(false);

    statusLayout->addWidget(progressBar);
    ui->containerTable->setCellWidget(row, 3, statusWidget);

    QProcess *process = new QProcess(this);
    process->setProcessEnvironment(env);
    process->setProcessChannelMode(QProcess::MergedChannels);

    activeProcesses[row] = process;

    connect(process, &QProcess::readyRead, this, [this, process]() {
        QString output = process->readAll().trimmed();
        ui->terminal->appendPlainText(output);

        ui->terminal->verticalScrollBar()->setValue(ui->terminal->verticalScrollBar()->maximum());
    });

    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, [this, row, process, command]() {
        if (activeProcesses.contains(row)) {
            activeProcesses.remove(row);
        }

        populateContainerData(); // Refresh table data from Python script

        process->deleteLater();
    });

    process->start(pythonPath, QStringList() << scriptPath << imageName << command);
}

void ContainerTable::adjustColumnWidths()
{
    for (int i = 0; i < ui->containerTable->columnCount(); ++i) {
        ui->containerTable->resizeColumnToContents(i);
    }
}

void ContainerTable::displayLogInTerminal(const QString &log)
{
    ui->terminal->appendPlainText(log);

    QScrollBar *scrollBar = ui->terminal->verticalScrollBar();
    scrollBar->setValue(scrollBar->maximum());
}
