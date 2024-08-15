#include "mainbuttons.h"
#include "mainwindow.h"

MainButtons::MainButtons(Ui::MainWindow *ui, QObject *parent)
    : QObject(parent), ui(ui)
{
}

void MainButtons::setupConnections(QObject *mainWindow)
{
    auto mainWin = qobject_cast<MainWindow*>(mainWindow);
    if (!mainWin) return;

    connect(ui->dashboardButton, &QPushButton::clicked, mainWin, &MainWindow::showDashboard);
    connect(ui->settingsButton, &QPushButton::clicked, mainWin, &MainWindow::showSettings);
    connect(ui->accountButton, &QPushButton::clicked, mainWin, &MainWindow::showAccount);
    connect(ui->hubButton, &QPushButton::clicked, mainWin, &MainWindow::showHub);
    connect(ui->helpButton, &QPushButton::clicked, mainWin, &MainWindow::showHelpPage); // Connect the help button
}
