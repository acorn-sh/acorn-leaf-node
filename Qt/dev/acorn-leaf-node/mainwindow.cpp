#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "helppage.h"
#include <QTimer>
#include <QTime>
#include <QProcessEnvironment>
#include <QProcess>
#include <QCoreApplication>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setFixedSize(1100, 600);

    mainButtons = new MainButtons(ui);
    containerTable = new ContainerTable(ui);
    terminal = new Terminal(ui);
    settingsPage = new SettingsPage(ui);
    hubPage = new HubPage(ui);

    setupUI();
    setupConnections();

    ui->stackedWidget->setCurrentWidget(ui->dashboardPage);

    // Show the HelpPage as a modal popup if it's the first time or if "Do not show again" is not checked
    if (HelpPage::shouldShowHelp()) {
        showHelpPage();  // Reuse the showHelpPage method
    }

    // Timer to update runtime every second
    QTimer *runtimeTimer = new QTimer(this);
    connect(runtimeTimer, &QTimer::timeout, this, &MainWindow::updateRuntime);
    runtimeTimer->start(1000);

    // Timer to update account balance every minute
    balanceUpdateTimer = new QTimer(this);
    connect(balanceUpdateTimer, &QTimer::timeout, this, &MainWindow::updateAccountBalance);
    balanceUpdateTimer->start(60000); // 60,000 milliseconds = 1 minute

    // Initial call to update the account balance when the application starts
    updateAccountBalance();
}

void MainWindow::showHelpPage() {
    HelpPage helpPage(this);
    helpPage.setModal(true);

    // Center the HelpPage over the MainWindow
    int x = this->geometry().center().x() - helpPage.width() / 2;
    int y = this->geometry().center().y() - helpPage.height() / 2;
    helpPage.move(x, y);

    helpPage.exec();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete mainButtons;
    delete containerTable;
    delete terminal;
    delete settingsPage;
    delete hubPage;

}

void MainWindow::setupUI()
{
    terminal->setupTerminal();
}

void MainWindow::setupConnections()
{
    mainButtons->setupConnections(this);
    settingsPage->setupConnections(this);
    hubPage->setupConnections(this);  // Setup connections for HubPage

}

void MainWindow::showDashboard()
{
    ui->stackedWidget->setCurrentWidget(ui->dashboardPage);
}

void MainWindow::showHub()
{
    ui->stackedWidget->setCurrentWidget(ui->hubPage);
    hubPage->populateHubTable();  // Populate the table when HubPage is shown

}

void MainWindow::showSettings()
{
    ui->stackedWidget->setCurrentWidget(ui->settingsPage);
}



void MainWindow::showAccount()
{
    ui->stackedWidget->setCurrentWidget(ui->accountPage);
}

void MainWindow::updateRuntime()
{
    containerTable->updateRuntime();
}

void MainWindow::updateAccountBalance()
{
    QString address = "0xDf6e59c6DF1E9500fd35A76FF4C62F9901E90019";

    QProcess *process = new QProcess(this);
    connect(process, &QProcess::readyReadStandardOutput, this, [this, process]() {
        QByteArray output = process->readAllStandardOutput();
        QString balance = QString(output).trimmed();
        if (balance.isEmpty()) {
            ui->accountButton->setText("Account: Failed to retrieve balance");
        } else {
            ui->accountButton->setText(balance);
        }
    });
    connect(process, &QProcess::readyReadStandardError, this, [this, process]() {
        QByteArray errorOutput = process->readAllStandardError();
        QString errorString = QString(errorOutput).trimmed();
        qDebug() << "Error:" << errorString;
        QMessageBox::critical(nullptr, "Error", "Failed to retrieve balance:\n" + errorString);
    });

    QString pythonPath = QCoreApplication::applicationDirPath() + "/bundled_python/bin/python3.11";
    process->start(pythonPath, QStringList() << QCoreApplication::applicationDirPath() + "/check_balance.py" << address);
}

