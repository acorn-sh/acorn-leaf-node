#include "containertable.h"
#include <QTime>
#include <QProcess>
#include <QDebug>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QPushButton>
#include <QRegularExpression>
#include <QScrollBar>

ContainerTable::ContainerTable(Ui::MainWindow *ui) : ui(ui)
{
    setupTable();
    populateContainerData();
}

void ContainerTable::setupTable()
{
    // Updated headers list with PORT after NET
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

    // Set up the environment variables to ensure the Docker command is found
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    QString path = env.value("PATH");
    if (!path.contains("/usr/local/bin")) {
        path = "/usr/local/bin:" + path;
        env.insert("PATH", path);
    }

    QProcess *process = new QProcess(this);
    process->setProcessEnvironment(env);
    process->setProcessChannelMode(QProcess::MergedChannels);

    // Define the path to Python and the script
    QString pythonPath = QCoreApplication::applicationDirPath() + "/bundled_python/bin/python3.11";
    QString scriptPath = QCoreApplication::applicationDirPath() + "/list_docker_image.py";

    connect(process, &QProcess::readyRead, [process, &sudoacornData, this]() {
        QString output = process->readAll();
        displayLogInTerminal(output);  // Display output in terminal
        sudoacornData = output.split("\n", Qt::SkipEmptyParts);
    });

    process->start(pythonPath, QStringList() << scriptPath);
    process->waitForFinished();

    ui->containerTable->setRowCount(sudoacornData.size());

    for (int i = 0; i < sudoacornData.size(); ++i) {
        QStringList details = sudoacornData[i].split("|", Qt::SkipEmptyParts);

        if (details.size() < 9) continue;  // Adjust for new PORT column

        QString repository = details[0].trimmed();
        QString imageId = details[1].trimmed();
        QString containerId = details[2].trimmed();
        QString status = details[3].trimmed();
        QString time = details[4].trimmed();  // Actual uptime

        QString cpu = details[5].trimmed();
        QString gpu = "";  // Placeholder for GPU usage if needed
        QString mem_usage = details[6].trimmed();
        QString net_io = details[7].trimmed();
        QString port = details[8].trimmed();  // Port info

        ui->containerTable->setItem(i, 0, new QTableWidgetItem(repository));
        ui->containerTable->setItem(i, 1, new QTableWidgetItem(imageId));
        ui->containerTable->setItem(i, 2, new QTableWidgetItem(containerId));
        ui->containerTable->setItem(i, 3, new QTableWidgetItem(status));
        ui->containerTable->setItem(i, 4, new QTableWidgetItem(time));
        ui->containerTable->setItem(i, 5, new QTableWidgetItem(cpu));
        ui->containerTable->setItem(i, 6, new QTableWidgetItem(gpu));
        ui->containerTable->setItem(i, 7, new QTableWidgetItem(mem_usage));
        ui->containerTable->setItem(i, 8, new QTableWidgetItem(net_io));
        ui->containerTable->setItem(i, 9, new QTableWidgetItem(port));  // Add port column

        addControlButtons(i, status);
    }

    adjustColumnWidths();
}

void ContainerTable::addControlButtons(int row, const QString &activeStatus)
{
    QWidget* pWidget = new QWidget();
    QHBoxLayout* pButtonLayout = new QHBoxLayout(pWidget);  // Horizontal layout for the buttons
    pButtonLayout->setAlignment(Qt::AlignCenter);
    pButtonLayout->setContentsMargins(0, 0, 0, 0); // Ensure there's no unnecessary padding
    pButtonLayout->setSpacing(5);

    // Create buttons with icons and apply the stylesheet
    QPushButton* btnStart = new QPushButton();
    QPushButton* btnStop = new QPushButton();
    QPushButton* btnPause = new QPushButton();
    QPushButton* btnRestart = new QPushButton();

    btnStart->setIcon(QIcon(":/images/icons/play.svg"));
    btnStop->setIcon(QIcon(":/images/icons/stop.svg"));
    btnPause->setIcon(QIcon(":/images/icons/pause.svg"));
    btnRestart->setIcon(QIcon(":/images/icons/restart.svg"));

    btnStart->setFixedSize(24, 24);
    btnStop->setFixedSize(24, 24);
    btnPause->setFixedSize(24, 24);
    btnRestart->setFixedSize(24, 24);

    // Apply the same stylesheet as defined in your application to ensure consistency
    QString buttonStyle = "QPushButton { background-color: #938ea4; }"
                          "QPushButton:hover { background-color: #fbdea3; }"
                          "QPushButton:pressed { background-color: #6d6781; }";

    btnStart->setStyleSheet(buttonStyle);
    btnStop->setStyleSheet(buttonStyle);
    btnPause->setStyleSheet(buttonStyle);
    btnRestart->setStyleSheet(buttonStyle);

    // Connect signals to slots for handling button actions
    connect(btnStart, &QPushButton::clicked, this, [this, row](){ handleDockerCommand(row, "start"); });
    connect(btnStop, &QPushButton::clicked, this, [this, row](){ handleDockerCommand(row, "stop"); });
    connect(btnPause, &QPushButton::clicked, this, [this, row](){ handleDockerCommand(row, "pause"); });
    connect(btnRestart, &QPushButton::clicked, this, [this, row](){ handleDockerCommand(row, "restart"); });

    // Add buttons to the layout
    pButtonLayout->addWidget(btnStart);
    pButtonLayout->addWidget(btnStop);
    pButtonLayout->addWidget(btnPause);
    pButtonLayout->addWidget(btnRestart);

    // Set the layout for the widget and place it in the table
    pWidget->setLayout(pButtonLayout);
    ui->containerTable->setCellWidget(row, 3, pWidget);
}


void ContainerTable::updateRuntime()
{
    for (int i = 0; i < ui->containerTable->rowCount(); ++i) {
        QTableWidgetItem *statusItem = ui->containerTable->item(i, 3); // Status column
        QTableWidgetItem *timeItem = ui->containerTable->item(i, 4);   // Time column
        if (statusItem && statusItem->text() == "Running") {
            QTime time = QTime::fromString(timeItem->text(), "hh:mm:ss");
            time = time.addSecs(1);
            timeItem->setText(time.toString("hh:mm:ss"));
        }
    }
}

void ContainerTable::handleDockerCommand(int row, const QString &command)
{
    QString imageName = ui->containerTable->item(row, 0)->text();  // Assuming the image name is the repository
    qDebug() << "Attempting to run command" << command << "for image" << imageName;

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    QString path = env.value("PATH");
    if (!path.contains("/usr/local/bin")) {
        path = "/usr/local/bin:" + path;
        env.insert("PATH", path);
    }

    QProcess *process = new QProcess(this);
    process->setProcessEnvironment(env);
    process->setProcessChannelMode(QProcess::MergedChannels);

    QString pythonPath = QCoreApplication::applicationDirPath() + "/bundled_python/bin/python3.11";
    QString scriptPath = QCoreApplication::applicationDirPath() + "/start_docker_image.py";

    activeProcesses[row] = process;

    connect(process, &QProcess::readyRead, this, [this, process]() {
        QString output = process->readAll().trimmed();
        qDebug() << "Docker script output:" << output;
        ui->terminal->appendPlainText(output);  // Display output in the terminal

        // Ensure terminal always scrolls to the bottom when new output is added.
        ui->terminal->verticalScrollBar()->setValue(ui->terminal->verticalScrollBar()->maximum());
    });

    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, [this, row, process, command](int exitCode, QProcess::ExitStatus exitStatus) {
        qDebug() << "Process finished with exit code:" << exitCode << "and status:" << exitStatus;

        if (exitCode == 0 && exitStatus == QProcess::NormalExit) {
            QTableWidgetItem* statusItem = ui->containerTable->item(row, 3); // Status column
            QTableWidgetItem* timeItem = ui->containerTable->item(row, 4);   // Time column
            if (statusItem) {
                if (command == "start") {
                    statusItem->setText("Running");
                    timeItem->setText("00:00:00");
                } else if (command == "stop") {
                    statusItem->setText("Stopped");
                    timeItem->setText("");
                }
            }
        } else {
            qDebug() << "Failed to execute the command. Exit code:" << exitCode << "Error status:" << exitStatus;
            QMessageBox::critical(nullptr, "Docker Script Error", "Failed to execute the Docker command.");
        }

        activeProcesses.remove(row);
    });

    qDebug() << "Starting process" << pythonPath << scriptPath << imageName << command;
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

    // Scroll to the bottom of the terminal
    QScrollBar *scrollBar = ui->terminal->verticalScrollBar();
    scrollBar->setValue(scrollBar->maximum());
}
