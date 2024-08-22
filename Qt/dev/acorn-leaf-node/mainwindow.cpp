#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "helppage.h"
#include "accountpage.h"

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
    accountPage = new AccountPage(ui, this);

    setupUI();
    setupConnections();

    ui->stackedWidget->setCurrentWidget(ui->dashboardPage);

    this->show();

    if (HelpPage::shouldShowHelp()) {
        QTimer::singleShot(100, this, &MainWindow::showHelpPage);
    }

    QTimer *runtimeTimer = new QTimer(this);
    connect(runtimeTimer, &QTimer::timeout, this, &MainWindow::updateRuntime);
    runtimeTimer->start(1000);

    balanceUpdateTimer = new QTimer(this);
    connect(balanceUpdateTimer, &QTimer::timeout, this, &MainWindow::updateAccountBalance);
    balanceUpdateTimer->start(60000);
    updateAccountBalance();
}

void MainWindow::showHelpPage() {
    HelpPage helpPage(this);
    helpPage.setModal(true);

    helpPage.resize(this->width() * 0.8, this->height() * 0.8);
    helpPage.move(this->geometry().center() - helpPage.rect().center());

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
    delete accountPage;
}

void MainWindow::setupUI()
{
    terminal->setupTerminal();
    ui->terminal->setMaximumHeight(38);
}

void MainWindow::setupConnections()
{
    mainButtons->setupConnections(this);
    settingsPage->setupConnections(this);
    hubPage->setupConnections(this);
    accountPage->setupConnections();

    connect(ui->expandButton, &QPushButton::clicked, this, &MainWindow::toggleTerminalExpansion);
}

void MainWindow::showDashboard()
{
    ui->stackedWidget->setCurrentWidget(ui->dashboardPage);
}

void MainWindow::showHub()
{
    ui->stackedWidget->setCurrentWidget(ui->hubPage);
    hubPage->populateHubTable();
}

void MainWindow::showSettings()
{
    ui->stackedWidget->setCurrentWidget(ui->settingsPage);
}

void MainWindow::showAccount()
{
    ui->stackedWidget->setCurrentWidget(ui->accountPage);
    accountPage->refreshEthereumAddress();
}

void MainWindow::updateRuntime()
{
    //containerTable->updateRuntime();
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

void MainWindow::toggleTerminalExpansion()
{
    if (ui->terminal->maximumHeight() == 38) {
        ui->terminal->setMaximumHeight(400);
        ui->expandButton->setIcon(QIcon(":/images/icons/chevron-down.svg"));
    } else {
        ui->terminal->setMaximumHeight(38);
        ui->expandButton->setIcon(QIcon("://images/icons/chevron-up.svg"));
    }
}

