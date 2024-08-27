#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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
};

#endif // MAINWINDOW_H
