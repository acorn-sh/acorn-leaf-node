#include "HubTable.h"
#include <QJsonDocument>
#include <QJsonObject>

HubTable::HubTable(Ui::MainWindow* ui, ResourceManager* resourceManager, QObject* parent)
    : QObject(parent), ui(ui), resourceManager(resourceManager)
{
    components = new Components(resourceManager);
    setupTable();
}

void HubTable::setupTable()
{
    QStringList headers = {"Repository Name", "Description", "Stars", "Link", "Status"};
    ui->hubTableWidget->setColumnCount(headers.size());
    ui->hubTableWidget->setHorizontalHeaderLabels(headers);
    ui->hubTableWidget->setStyleSheet("background-color: #f6f2e8; color: black;");

    ui->hubTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->hubTableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->hubTableWidget->setSelectionBehavior(QAbstractItemView::SelectItems);
    ui->hubTableWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    for (int i = 0; i < headers.size(); ++i) {
        ui->hubTableWidget->horizontalHeader()->setSectionResizeMode(i, QHeaderView::ResizeToContents);
    }
}

void HubTable::populateHubTable(const QStringList &repos)
{
    ui->hubTableWidget->setRowCount(0);  // Clear any existing rows

    for (int i = 0; i < repos.size(); ++i) {
        QJsonDocument doc = QJsonDocument::fromJson(repos[i].toUtf8());
        QJsonObject repoDetails = doc.object();

        ui->hubTableWidget->insertRow(i);  // Insert a new row

        ui->hubTableWidget->setItem(i, 0, new QTableWidgetItem(repoDetails["name"].toString()));
        ui->hubTableWidget->setItem(i, 1, new QTableWidgetItem(repoDetails["description"].toString()));
        ui->hubTableWidget->setItem(i, 2, new QTableWidgetItem(QString::number(repoDetails["stars"].toInt())));
        ui->hubTableWidget->setItem(i, 3, new QTableWidgetItem(repoDetails["link"].toString()));

        addControlButtons(i);
    }
}

void HubTable::addControlButtons(int row)
{
    QWidget* controlWidget = new QWidget();
    QHBoxLayout* controlLayout = new QHBoxLayout(controlWidget);
    controlLayout->setContentsMargins(0, 0, 0, 0);
    controlLayout->setAlignment(Qt::AlignCenter);
    controlLayout->setSpacing(5);

    QPushButton* btnInstall = components->createButtonWithIcon("download");
    QPushButton* btnStart = components->createButtonWithIcon("start");
    QPushButton* btnDelete = components->createButtonWithIcon("delete");

    controlLayout->addWidget(btnInstall);
    controlLayout->addWidget(btnStart);
    controlLayout->addWidget(btnDelete);

    controlWidget->setLayout(controlLayout);
    ui->hubTableWidget->setCellWidget(row, 4, controlWidget);

    connect(btnInstall, &QPushButton::clicked, this, [this, row]() { emit installRequested(row); });
    connect(btnStart, &QPushButton::clicked, this, [this, row]() { emit startRequested(row); });
    connect(btnDelete, &QPushButton::clicked, this, [this, row]() { emit deleteRequested(row); });
}

void HubTable::replaceButtonsWithProgressBar(int row)
{
    QWidget* progressWidget = components->createProgressBarWidget(1, ui->hubTableWidget);
    ui->hubTableWidget->setCellWidget(row, 4, progressWidget);
}

QString HubTable::getImageIDs(const QString &repoName)
{
    QProcess process;
    process.setProcessEnvironment(QProcessEnvironment::systemEnvironment());
    process.start("docker", QStringList() << "images" << "--format" << "{{.ID}}" << repoName);
    process.waitForFinished();

    if (process.exitCode() != 0) {
        qDebug() << "Failed to fetch image ID. Exit code:" << process.exitCode();
        qDebug() << "Error output:" << process.readAllStandardError().trimmed();
        return QString();
    }

    QString output = process.readAllStandardOutput().trimmed();
    return output.split(":").last();
}
