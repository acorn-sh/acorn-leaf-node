#include "HubPage.h"
#include "ResourceManager.h"
#include "Terminal.h"

#include <QDebug>
#include <QPushButton>
#include <QHBoxLayout>
#include <QWidget>
#include <QProgressBar>
#include <QMessageBox>
#include <QProcess>
#include <QScrollBar>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

HubPage::HubPage(Ui::MainWindow *ui, ResourceManager *resourceManager, QObject *parent)
    : QObject(parent), ui(ui), resourceManager(resourceManager), terminal(new Terminal(ui->terminal, ui->expandButton, this))
{
    setupTable();
    populateHubTable();
}

HubPage::~HubPage() {
    delete terminal;
}

void HubPage::setupTable()
{
    QStringList headers = {"Repository Name", "Description", "Stars", "Link", "Status"};
    ui->hubTableWidget->setColumnCount(headers.size());
    ui->hubTableWidget->setHorizontalHeaderLabels(headers);

    ui->hubTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->hubTableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->hubTableWidget->setSelectionBehavior(QAbstractItemView::SelectItems);
    ui->hubTableWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);


    for (int i = 0; i < headers.size(); ++i) {
        ui->hubTableWidget->horizontalHeader()->setSectionResizeMode(i, QHeaderView::ResizeToContents);
    }

    adjustColumnWidths();
}

void HubPage::adjustColumnWidths()
{
    ui->hubTableWidget->resizeColumnToContents(0);
    ui->hubTableWidget->resizeColumnToContents(1);
    ui->hubTableWidget->resizeColumnToContents(2);
    ui->hubTableWidget->resizeColumnToContents(3);

    int totalWidth = ui->hubTableWidget->viewport()->width();
    int fixedColumnsWidth = ui->hubTableWidget->columnWidth(0) + ui->hubTableWidget->columnWidth(1) + ui->hubTableWidget->columnWidth(2) + ui->hubTableWidget->columnWidth(3);
    int remainingWidth = totalWidth - fixedColumnsWidth;
    int variableColumnsCount = ui->hubTableWidget->columnCount() - 4;

    int columnWidth = remainingWidth / variableColumnsCount;

    for (int i = 4; i < ui->hubTableWidget->columnCount(); ++i) {
        ui->hubTableWidget->setColumnWidth(i, std::max(columnWidth, 50));
    }
}

void HubPage::populateHubTable()
{
    QStringList repos = getDockerHubRepos();

    ui->hubTableWidget->setRowCount(0);  // Clear any existing rows before populating new data

    for (int i = 0; i < repos.size(); ++i) {
        QJsonDocument doc = QJsonDocument::fromJson(repos[i].toUtf8());
        QJsonObject repoDetails = doc.object();

        ui->hubTableWidget->insertRow(i);  // Insert a new row

        ui->hubTableWidget->setItem(i, 0, new QTableWidgetItem(repoDetails["name"].toString()));
        ui->hubTableWidget->setItem(i, 1, new QTableWidgetItem(repoDetails["description"].toString()));
        ui->hubTableWidget->setItem(i, 2, new QTableWidgetItem(QString::number(repoDetails["stars"].toInt())));
        ui->hubTableWidget->setItem(i, 3, new QTableWidgetItem(repoDetails["link"].toString()));

        addControlButtons(i);  // Add permanent install, start, and delete buttons
    }

    adjustColumnWidths();
}

QStringList HubPage::getDockerHubRepos()
{
    QString pythonPath = resourceManager->getPythonPath();
    QString scriptPath = resourceManager->getScriptPath("docker_images.py");

    QProcess process;
    process.setProcessEnvironment(resourceManager->getProcessEnvironment());
    process.start(pythonPath, QStringList() << scriptPath << "-c" << "get_sudoacorn_repos");
    process.waitForFinished();

    if (process.exitCode() != 0) {
        qDebug() << "Failed to execute script. Exit code:" << process.exitCode();
        qDebug() << "Error output:" << process.readAllStandardError().trimmed();
        return QStringList();
    }

    QString output = process.readAllStandardOutput().trimmed();
    qDebug() << "Script output:" << output;

    QStringList repos;
    QJsonDocument doc = QJsonDocument::fromJson(output.toUtf8());
    if (doc.isArray()) {
        QJsonArray array = doc.array();
        for (const QJsonValue &value : array) {
            repos.append(QString::fromUtf8(QJsonDocument(value.toObject()).toJson(QJsonDocument::Compact)));
        }
    }

    return repos;
}

void HubPage::addControlButtons(int row)
{
    QWidget* controlWidget = new QWidget();
    QHBoxLayout* controlLayout = new QHBoxLayout(controlWidget);
    controlLayout->setContentsMargins(0, 0, 0, 0);
    controlLayout->setAlignment(Qt::AlignCenter);
    controlLayout->setSpacing(5);

    QPushButton* btnInstall = createControlButton("download", "download", row);
    QPushButton* btnStart = createControlButton("start", "start", row);
    QPushButton* btnDelete = createControlButton("delete", "delete", row);

    controlLayout->addWidget(btnInstall);
    controlLayout->addWidget(btnStart);
    controlLayout->addWidget(btnDelete);

    controlWidget->setLayout(controlLayout);
    ui->hubTableWidget->setCellWidget(row, 4, controlWidget);
}

QPushButton* HubPage::createControlButton(const QString &iconName, const QString &action, int row)
{
    QPushButton* button = new QPushButton();
    button->setFixedSize(24, 24);
    button->setIcon(resourceManager->getIcon(iconName));

    QString buttonStyle = "QPushButton { background-color: #938ea4; }"
                          "QPushButton:hover { background-color: #fbdea3; }"
                          "QPushButton:pressed { background-color: #6d6781; }";
    button->setStyleSheet(buttonStyle);

    if (action == "download") {
        connect(button, &QPushButton::clicked, this, [this, row]() { handleInstall(row); });
    } else if (action == "start") {
        connect(button, &QPushButton::clicked, this, [this, row]() { handleStart(row); });
    } else if (action == "delete") {
        connect(button, &QPushButton::clicked, this, [this, row]() { handleDelete(row); });
    }

    return button;
}

void HubPage::handleInstall(int row)
{
    QString url = ui->hubTableWidget->item(row, 3)->text();

    replaceButtonsWithProgressBar(row);

    QProcessEnvironment env = resourceManager->getProcessEnvironment();
    QProcess* process = new QProcess(this);
    process->setProcessEnvironment(env);

    QString pythonPath = resourceManager->getPythonPath();
    QString scriptPath = resourceManager->getScriptPath("docker_images.py");

    activeProcesses[row] = process;

    connect(process, &QProcess::readyReadStandardOutput, this, [this, process]() {
        QString output = process->readAllStandardOutput().trimmed();
        terminal->appendLog(output);
    });

    connect(process, &QProcess::readyReadStandardError, this, [this, process]() {
        QString error = process->readAllStandardError().trimmed();
        terminal->appendLog(error);
    });

    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, [this, row, process](int exitCode, QProcess::ExitStatus exitStatus) {
        if (exitCode == 0 && exitStatus == QProcess::NormalExit) {
            terminal->appendLog("Docker image successfully pulled.");
        } else {
            QMessageBox::critical(nullptr, "Installation Error", "Failed to pull the Docker image.");
        }

        addControlButtons(row);  // Re-add the buttons after completion
        activeProcesses.remove(row);
    });

    process->start(pythonPath, QStringList() << scriptPath << "-c" << "pull" << url);
}

void HubPage::handleStart(int row)
{
    QString repoName = ui->hubTableWidget->item(row, 0)->text().trimmed();

    replaceButtonsWithProgressBar(row);

    QProcessEnvironment env = resourceManager->getProcessEnvironment();
    QProcess* process = new QProcess(this);
    process->setProcessEnvironment(env);

    QString pythonPath = resourceManager->getPythonPath();
    QString scriptPath = resourceManager->getScriptPath("docker_images.py");

    QString imageID;
    QProcess fetchImageIDProcess;
    fetchImageIDProcess.setProcessEnvironment(env);
    fetchImageIDProcess.start(pythonPath, QStringList() << scriptPath << "-c" << "get_image_id" << repoName);
    fetchImageIDProcess.waitForFinished();

    if (fetchImageIDProcess.exitCode() == 0) {
        imageID = fetchImageIDProcess.readAllStandardOutput().trimmed();
        if (imageID.isEmpty()) {
            QMessageBox::critical(nullptr, "Error", "Failed to retrieve image ID.");
            ui->hubTableWidget->removeCellWidget(row, 5);
            addControlButtons(row);
            return;
        }
    } else {
        QMessageBox::critical(nullptr, "Error", "Failed to fetch image ID.");
        ui->hubTableWidget->removeCellWidget(row, 5);
        addControlButtons(row);
        return;
    }

    activeProcesses[row] = process;

    connect(process, &QProcess::readyReadStandardOutput, this, [this, process]() {
        QString output = process->readAllStandardOutput().trimmed();
        terminal->appendLog(output);
    });

    connect(process, &QProcess::readyReadStandardError, this, [this, process]() {
        QString error = process->readAllStandardError().trimmed();
        terminal->appendLog(error);
    });

    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, [this, row, process](int exitCode, QProcess::ExitStatus exitStatus) {
        if (exitCode == 0 && exitStatus == QProcess::NormalExit) {
            terminal->appendLog("Docker container started successfully.");
        } else {
            QMessageBox::critical(nullptr, "Start Error", "Failed to start the Docker container.");
        }

        ui->hubTableWidget->removeCellWidget(row, 5);  // Remove the progress bar
        addControlButtons(row);  // Re-add the buttons after completion
        activeProcesses.remove(row);
    });

    process->start(pythonPath, QStringList() << scriptPath << "-c" << "start" << imageID);
}

void HubPage::handleDelete(int row)
{
    QString url = ui->hubTableWidget->item(row, 3)->text();

    replaceButtonsWithProgressBar(row);

    QProcessEnvironment env = resourceManager->getProcessEnvironment();
    QProcess* process = new QProcess(this);
    process->setProcessEnvironment(env);

    QString pythonPath = resourceManager->getPythonPath();
    QString scriptPath = resourceManager->getScriptPath("docker_images.py");

    activeProcesses[row] = process;

    connect(process, &QProcess::readyReadStandardOutput, this, [this, process]() {
        QString output = process->readAllStandardOutput().trimmed();
        terminal->appendLog(output);
    });

    connect(process, &QProcess::readyReadStandardError, this, [this, process]() {
        QString error = process->readAllStandardError().trimmed();
        terminal->appendLog(error);
    });

    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, [this, row, process](int exitCode, QProcess::ExitStatus exitStatus) {
        if (exitCode == 0 && exitStatus == QProcess::NormalExit) {
            terminal->appendLog("Successfully deleted Docker image.");
            ui->hubTableWidget->removeRow(row);
        } else {
            QMessageBox::critical(nullptr, "Deletion Error", "Failed to delete the Docker image.");
        }

        activeProcesses.remove(row);
    });

    process->start(pythonPath, QStringList() << scriptPath << "-c" << "delete" << url);
}

QString HubPage::fetchImageID(const QString &repoName)
{
    QString pythonPath = resourceManager->getPythonPath();
    QString scriptPath = resourceManager->getScriptPath("docker_images.py");

    QProcess process;
    process.setProcessEnvironment(resourceManager->getProcessEnvironment());
    process.start(pythonPath, QStringList() << scriptPath << "-c" << "get_image_id" << repoName);
    process.waitForFinished();

    if (process.exitCode() != 0) {
        qDebug() << "Failed to fetch image ID. Exit code:" << process.exitCode();
        qDebug() << "Error output:" << process.readAllStandardError().trimmed();
        return QString();
    }

    return process.readAllStandardOutput().trimmed();
}

void HubPage::replaceButtonsWithProgressBar(int row)
{
    QWidget* progressWidget = new QWidget();
    QHBoxLayout* layout = new QHBoxLayout(progressWidget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setAlignment(Qt::AlignCenter);

    QProgressBar* progressBar = new QProgressBar();
    progressBar->setRange(0, 0);
    progressBar->setFixedHeight(1);
    progressBar->setTextVisible(false);

    layout->addWidget(progressBar);
    progressWidget->setLayout(layout);

    ui->hubTableWidget->setCellWidget(row, 4, progressWidget);
}
