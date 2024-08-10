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
    connect(ui->balanceButton, &QPushButton::clicked, mainWin, &MainWindow::showBalance);
}
