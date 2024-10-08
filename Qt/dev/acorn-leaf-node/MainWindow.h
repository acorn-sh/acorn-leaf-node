#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "TrayIcon.h"

class ResourceManager;
class PythonServiceManager;
class BlockchainManager;
class PaymentManager;
class MainButtons;
class Terminal;
class SettingsPage;
class AccountPage;
class DashboardPage;
class DashboardTable;
class HubPage;
class HubTable;
class QTimer;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;

public slots:
    void showDashboard();
    void showSettings();
    void showAccount();
    void showHub();
    void updateRuntime();
    void showHelpPage();
    void refreshDashboard();
    void refreshHub();

private slots:
    void setupUI();
    void setupConnections();
    void updateAccountBalance();
    void toggleTerminalExpansion();

    void minimizeToTray();
    void restoreFromTray();

private:
    Ui::MainWindow *ui;
    ResourceManager *resourceManager;
    PythonServiceManager *pythonServiceManager;
    BlockchainManager *blockchainManager;
    PaymentManager *paymentManager;
    MainButtons *mainButtons;
    DashboardTable *dashboardTable;
    DashboardPage *dashboardPage;
    Terminal *terminal;
    SettingsPage *settingsPage;
    HubPage *hubPage;
    HubTable *hubTable;
    AccountPage *accountPage;
    QTimer *balanceUpdateTimer;

    TrayIcon *trayIcon;
};

#endif // MAINWINDOW_H
