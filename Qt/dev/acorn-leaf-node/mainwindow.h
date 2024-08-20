#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "mainbuttons.h"
#include "containertable.h"
#include "terminal.h"
#include "settingspage.h"
#include "hubpage.h"
#include "accountpage.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void showDashboard();
    void showSettings();
    void showAccount();
    void showHub();
    void updateRuntime();
    void showHelpPage();

private slots:
    void updateAccountBalance();

private:
    Ui::MainWindow *ui;
    MainButtons *mainButtons;
    ContainerTable *containerTable;
    Terminal *terminal;
    SettingsPage *settingsPage;
    QTimer *balanceUpdateTimer;
    HubPage *hubPage;
    AccountPage *accountPage;
    void toggleTerminalExpansion();
    void toggleTerminalExpansionHub();

    void setupUI();
    void setupConnections();
};

#endif // MAINWINDOW_H
