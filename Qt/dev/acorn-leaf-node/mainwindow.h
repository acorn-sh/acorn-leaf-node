#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "mainbuttons.h"
#include "containertable.h"
#include "terminal.h"
#include "settingspage.h"

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
    void showBalance();
    void updateRuntime();

private:
    Ui::MainWindow *ui;
    MainButtons *mainButtons;
    ContainerTable *containerTable;
    Terminal *terminal;
    SettingsPage *settingsPage;

    void setupUI();
    void setupConnections();
};

#endif // MAINWINDOW_H
