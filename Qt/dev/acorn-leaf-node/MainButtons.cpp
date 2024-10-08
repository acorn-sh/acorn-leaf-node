#include "MainButtons.h"
#include "MainWindow.h"

#include "QDesktopServices"

MainButtons::MainButtons(Ui::MainWindow *ui, QObject *parent)
    : QObject(parent), ui(ui)
{
}

void MainButtons::setupConnections(QObject *mainWindow)
{
    auto mainWin = qobject_cast<MainWindow*>(mainWindow);
    if (!mainWin) return;

    connect(ui->dashboardButton, &QPushButton::clicked, mainWin, &MainWindow::showDashboard);
    connect(ui->hubButton, &QPushButton::clicked, mainWin, &MainWindow::showHub);
    connect(ui->settingsButton, &QPushButton::clicked, mainWin, &MainWindow::showSettings);
    connect(ui->accountButton, &QPushButton::clicked, mainWin, &MainWindow::showAccount);
    connect(ui->refreshDashboardButton, &QPushButton::clicked, mainWin, &MainWindow::refreshDashboard);
    connect(ui->refreshHubButton, &QPushButton::clicked, mainWin, &MainWindow::refreshHub);
    connect(ui->helpButton, &QPushButton::clicked, mainWin, &MainWindow::showHelpPage);
    connect(ui->twitterButton, &QPushButton::clicked, this, []() {
        QUrl url("https://www.x.com/acorn_sh");
        QDesktopServices::openUrl(url);
    });
    connect(ui->acornshWebsiteButton, &QPushButton::clicked, this, []() {
        QUrl url("https://www.acorn.sh");
        QDesktopServices::openUrl(url);
    });

}
