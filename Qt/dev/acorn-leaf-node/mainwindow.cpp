#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTimer>
#include <QTime>

#include <QProcessEnvironment>
#include <QProcess>
#include <QCoreApplication>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setFixedSize(1100, 600); // Set fixed window size

    mainButtons = new MainButtons(ui);
    containerTable = new ContainerTable(ui);
    terminal = new Terminal(ui);
    settingsPage = new SettingsPage(ui);

    setupUI();
    setupConnections();

    // Set the initial page
    ui->stackedWidget->setCurrentWidget(ui->dashboardPage);

    // Timer to update runtime every second
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::updateRuntime);
    timer->start(1000); // Update every second
}

MainWindow::~MainWindow()
{
    delete ui;
    delete mainButtons;
    delete containerTable;
    delete terminal;
    delete settingsPage;
}

void MainWindow::setupUI()
{
    terminal->setupTerminal();
}

void MainWindow::setupConnections()
{
    mainButtons->setupConnections(this);
    settingsPage->setupConnections(this);
}

void MainWindow::showDashboard()
{
    ui->stackedWidget->setCurrentWidget(ui->dashboardPage);
}

void MainWindow::showSettings()
{
    ui->stackedWidget->setCurrentWidget(ui->settingsPage);
}

void MainWindow::showBalance()
{
    ui->stackedWidget->setCurrentWidget(ui->balancePage);
}

void MainWindow::updateRuntime()
{
    containerTable->updateRuntime();
}
