#include "MainWindow.h"
#include "HelpPage.h"
#include "AccountPage.h"
#include "DashboardPage.h"
#include "DashboardTable.h"
#include "ResourceManager.h"
#include "HubPage.h"
#include "HubTable.h"
#include "PythonServiceManager.h"
#include "BlockchainManager.h"
#include "PaymentManager.h"
#include "MainButtons.h"
#include "SettingsPage.h"
#include "Terminal.h"
#include "TrayIcon.h"

#include <QTimer>
#include <QTime>
#include <QProcessEnvironment>
#include <QProcess>
#include <QCoreApplication>
#include <QMessageBox>
#include <QtConcurrent/QtConcurrent>
#include <QIcon>
#include <QCloseEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), resourceManager(new ResourceManager()), trayIcon(new TrayIcon(resourceManager, this))
{
    ui->setupUi(this);
    this->setFixedSize(1100, 600);

    // TrayIcon
    connect(trayIcon, &TrayIcon::restoreRequested, this, &MainWindow::restoreFromTray);
    connect(trayIcon, &TrayIcon::quitRequested, qApp, &QApplication::quit);

    trayIcon->show();


    connect(ui->refreshDashboardButton, &QPushButton::clicked, dashboardPage, &DashboardPage::handleRefreshDashboard);

    resourceManager = new ResourceManager();
    pythonServiceManager = new PythonServiceManager(resourceManager);
    blockchainManager = new BlockchainManager(resourceManager);
    paymentManager = new PaymentManager(blockchainManager);

    mainButtons = new MainButtons(ui);

    dashboardTable = new DashboardTable(ui, resourceManager, this);
    dashboardPage = new DashboardPage(ui, resourceManager, this);

    hubTable = new HubTable(ui, resourceManager, this);
    hubPage = new HubPage(ui, resourceManager);

    settingsPage = new SettingsPage(ui, resourceManager);
    accountPage = new AccountPage(ui, resourceManager, this);

    terminal = new Terminal(ui->terminal, ui->expandButton, this);

    setupUI();
    setupConnections();

    (void)QtConcurrent::run([this]() {
        dashboardTable->getContainerStats();
        dashboardTable->getRunningContainers();
    });

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

MainWindow::~MainWindow()
{
    delete ui;
    delete mainButtons;
    delete dashboardPage;
    delete dashboardTable;
    delete terminal;
    delete settingsPage;
    delete hubPage;
    delete hubTable;
    delete accountPage;
    delete resourceManager;
    delete blockchainManager;
    delete paymentManager;
    delete pythonServiceManager;
    delete trayIcon;

}

void MainWindow::showHelpPage() {
    HelpPage helpPage(this);
    helpPage.setModal(true);

    helpPage.resize(this->width() * 0.8, this->height() * 0.8);
    helpPage.move(this->geometry().center() - helpPage.rect().center());

    helpPage.exec();
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
    accountPage->setupConnections();

    connect(ui->expandButton, &QPushButton::clicked, this, [this]() {
        terminal->toggleExpansion(resourceManager);
    });
}

void MainWindow::showDashboard()
{
    ui->stackedWidget->setCurrentWidget(ui->dashboardPage);
    (void)QtConcurrent::run([this]() {
        dashboardTable->getContainerStats();
        dashboardTable->getRunningContainers();
    });
}

void MainWindow::showHub()
{
    ui->stackedWidget->setCurrentWidget(ui->hubPage);
    (void)QtConcurrent::run([this]() {
        QStringList repos = hubPage->getDockerHubRepos();
        QMetaObject::invokeMethod(this, [this, repos]() {
            hubTable->populateHubTable(repos);
        }, Qt::QueuedConnection);
    });
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

void MainWindow::refreshHub()
{
    accountPage->refreshEthereumAddress();
}

void MainWindow::refreshDashboard()
{
    accountPage->refreshEthereumAddress();
}

void MainWindow::updateRuntime()
{
    //dashboardTable->updateRuntime();
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

void MainWindow::minimizeToTray()
{
    hide();
    trayIcon->showMessage("Acorn SH",
                                 "Application minimized to tray.",
                                 QSystemTrayIcon::Information,
                                 3000); // 3 seconds
}

void MainWindow::restoreFromTray()
{
    show();
    activateWindow();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    minimizeToTray();
    event->ignore();
}

void MainWindow::toggleTerminalExpansion()
{
    terminal->toggleExpansion(resourceManager);
}
