#include "hubpage.h"
#include <QProcess>
#include <QDebug>
#include <QPushButton>
#include <QHBoxLayout>
#include <QWidget>

HubPage::HubPage(Ui::MainWindow *ui, QObject *parent)
    : QObject(parent), ui(ui)
{
    setupTable();
    populateHubTable();
}

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

    // Ensure the table is read-only but selectable
    ui->hubTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->hubTableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->hubTableWidget->setSelectionBehavior(QAbstractItemView::SelectItems);

    // Allow horizontal scroll bar when necessary
    ui->hubTableWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    // Set initial widths to fit content for specific columns using ResizeToContents
    ui->hubTableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents); // Repository Name
    ui->hubTableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents); // Description
    ui->hubTableWidget->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents); // Stars
    ui->hubTableWidget->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents); // Link

    // Enable resizing by dragging for the Install and Status columns
    ui->hubTableWidget->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Interactive); // Install
    ui->hubTableWidget->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Interactive); // Status

    // Set minimum width for columns
    ui->hubTableWidget->horizontalHeader()->setMinimumSectionSize(70);

    // Adjust column widths to fit table width
    adjustColumnWidths();
}


void HubPage::adjustColumnWidths()
{
    // Resize specific columns to fit their contents
    ui->hubTableWidget->resizeColumnToContents(0); // Repository Name
    ui->hubTableWidget->resizeColumnToContents(1); // Description
    ui->hubTableWidget->resizeColumnToContents(2); // Stars
    ui->hubTableWidget->resizeColumnToContents(3); // Link

    // Adjust remaining columns to fit within the table width
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
    fetchDockerHubRepos();  // Fetch and populate the table with Docker Hub repositories
}

void HubPage::fetchDockerHubRepos()
{
    QString pythonPath = QCoreApplication::applicationDirPath() + "/bundled_python/bin/python3.11";
    QString scriptPath = QCoreApplication::applicationDirPath() + "/fetch_acornsh_docker_hub.py";

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

            addControlButtons(i); // Add control buttons to the table
        }
    }

    adjustColumnWidths();  // Adjust the column widths after populating the table
}

void HubPage::addControlButtons(int row)
{
    // Install button
    QPushButton* btnInstall = new QPushButton();
    btnInstall->setIcon(QIcon(":/images/install.png"));
    btnInstall->setFixedSize(24, 24);
    connect(btnInstall, &QPushButton::clicked, this, [this, row](){ handleInstall(row); });

    // Status button
    QPushButton* btnStatus = new QPushButton();
    btnStatus->setIcon(QIcon(":/images/status.png"));
    btnStatus->setFixedSize(24, 24);
    connect(btnStatus, &QPushButton::clicked, this, [this, row](){ handleStatus(row); });

    // Add the Install button to the Install column
    ui->hubTableWidget->setCellWidget(row, 4, btnInstall);

    // Add the Status button to the Status column
    ui->hubTableWidget->setCellWidget(row, 5, btnStatus);
}


void HubPage::handleInstall(int row) {
    // Implement what should happen when the install button is clicked
    qDebug() << "Install button clicked for row:" << row;
}

void HubPage::handleStatus(int row) {
    // Implement what should happen when the status button is clicked
    qDebug() << "Status button clicked for row:" << row;
}
