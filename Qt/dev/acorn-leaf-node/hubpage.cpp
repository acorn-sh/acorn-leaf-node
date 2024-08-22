#include "hubpage.h"
#include "ResourceManager.h"

#include <QDebug>
#include <QPushButton>
#include <QHBoxLayout>
#include <QWidget>
#include <QProgressBar>
#include <QMessageBox>
#include <QProcess>
#include <QScrollBar>

HubPage::HubPage(Ui::MainWindow *ui, QObject *parent)
    : QObject(parent), ui(ui), resourceManager(new ResourceManager)
{
    setupTable();
    populateHubTable();
}

// Remove the explicit destructor definition
// The compiler will automatically generate a destructor that correctly handles the deletion of resourceManager

void HubPage::setupConnections(QObject *mainWindow)
{
    // Set up any connections related to the HubPage
}

void HubPage::setupTable()
{
    QStringList headers = {"Repository Name", "Description", "Stars", "Link", "Install", "Status"};
    ui->hubTableWidget->setColumnCount(headers.size());
    ui->hubTableWidget->setHorizontalHeaderLabels(headers);
    ui->hubTableWidget->setStyleSheet("background-color: #d5d3e0; color: black;");

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
    fetchDockerHubRepos();
}

void HubPage::fetchDockerHubRepos()
{
    QString pythonPath = resourceManager->getPythonPath();
    QString scriptPath = resourceManager->getScriptPath("fetch_acornsh_docker_hub.py");

    QProcess process;
    process.start(pythonPath, QStringList() << scriptPath);
    process.waitForFinished();

    QString output = process.readAllStandardOutput().trimmed();
    QStringList lines = output.split("\n");

    ui->hubTableWidget->setRowCount(lines.size());

    for (int i = 0; i < lines.size(); ++i) {
        QStringList repoDetails = lines[i].split(", ");
        if (repoDetails.size() == 4) {
            ui->hubTableWidget->setItem(i, 0, new QTableWidgetItem(repoDetails[0].split(": ")[1]));
            ui->hubTableWidget->setItem(i, 1, new QTableWidgetItem(repoDetails[2].split(": ")[1]));
            ui->hubTableWidget->setItem(i, 2, new QTableWidgetItem(repoDetails[3].split(": ")[1]));
            ui->hubTableWidget->setItem(i, 3, new QTableWidgetItem(repoDetails[1].split(": ")[1]));

            addControlButtons(i);
        }
    }

    adjustColumnWidths();
}

void HubPage::addControlButtons(int row)
{
    QWidget* installWidget = new QWidget();
    QHBoxLayout* installLayout = new QHBoxLayout(installWidget);
    installLayout->setContentsMargins(0, 0, 0, 0);
    installLayout->setAlignment(Qt::AlignCenter);

    QPushButton* btnInstall = new QPushButton();
    btnInstall->setIcon(resourceManager->getIcon("download"));
    btnInstall->setFixedSize(24, 24);

    QString buttonStyle = "QPushButton { background-color: #938ea4; }"
                          "QPushButton:hover { background-color: #fbdea3; }"
                          "QPushButton:pressed { background-color: #6d6781; }";
    btnInstall->setStyleSheet(buttonStyle);

    installLayout->addWidget(btnInstall);

    connect(btnInstall, &QPushButton::clicked, this, [this, row](){ handleInstall(row); });

    ui->hubTableWidget->setCellWidget(row, 4, installWidget);

    QWidget* statusWidget = new QWidget();
    QHBoxLayout* statusLayout = new QHBoxLayout(statusWidget);
    statusLayout->setContentsMargins(0, 0, 0, 0);
    statusLayout->setAlignment(Qt::AlignCenter);

    ui->hubTableWidget->setCellWidget(row, 5, statusWidget);
}

void HubPage::handleInstall(int row) {
    QString url = ui->hubTableWidget->item(row, 3)->text();

    QProcessEnvironment env = resourceManager->getProcessEnvironment();

    QProcess *process = new QProcess(this);
    process->setProcessEnvironment(env);

    QString pythonPath = resourceManager->getPythonPath();
    QString scriptPath = resourceManager->getScriptPath("pull_docker_image.py");

    QWidget* statusWidget = new QWidget();
    QHBoxLayout* statusLayout = new QHBoxLayout(statusWidget);
    statusLayout->setContentsMargins(0, 0, 0, 0);
    statusLayout->setAlignment(Qt::AlignCenter);

    QProgressBar* progressBar = new QProgressBar();
    progressBar->setRange(0, 0);
    progressBar->setFixedHeight(1);
    progressBar->setTextVisible(false);

    statusLayout->addWidget(progressBar);
    ui->hubTableWidget->setCellWidget(row, 5, statusWidget);

    activeProcesses[row] = process;

    connect(process, &QProcess::readyReadStandardOutput, this, [this, process]() {
        QString output = process->readAllStandardOutput().trimmed();
        displayLogInTerminal(output);
    });

    connect(process, &QProcess::readyReadStandardError, this, [this, process]() {
        QString error = process->readAllStandardError().trimmed();
        displayLogInTerminal(error);
    });

    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, [this, row, process, progressBar](int exitCode, QProcess::ExitStatus exitStatus) {
        progressBar->setRange(0, 100);
        progressBar->setValue(100);

        if (exitCode == 0 && exitStatus == QProcess::NormalExit) {
            updateInstallButton(row);
        } else {
            QMessageBox::critical(nullptr, "Installation Error", "Failed to pull the Docker container.");
        }

        activeProcesses.remove(row);
    });

    process->start(pythonPath, QStringList() << scriptPath << url);
}

void HubPage::updateInstallButton(int row)
{
    QWidget* installWidget = new QWidget();
    QHBoxLayout* installLayout = new QHBoxLayout(installWidget);
    installLayout->setContentsMargins(0, 0, 0, 0);
    installLayout->setAlignment(Qt::AlignCenter);

    QPushButton* btnInstalled = new QPushButton();
    btnInstalled->setIcon(resourceManager->getIcon("check"));
    btnInstalled->setFixedSize(24, 24);
    installLayout->addWidget(btnInstalled);

    ui->hubTableWidget->setCellWidget(row, 4, installWidget);
}

void HubPage::displayLogInTerminal(const QString &log)
{
    ui->terminal->appendPlainText(log);
    QScrollBar *scrollBar = ui->terminal->verticalScrollBar();
    scrollBar->setValue(scrollBar->maximum());
}
