#ifndef SETTINGSPAGE_H
#define SETTINGSPAGE_H

#include <QObject>
#include "ui_mainwindow.h"
#include <QLabel>

class SettingsPage : public QObject
{
    Q_OBJECT

public:
    explicit SettingsPage(Ui::MainWindow *ui, QObject *parent = nullptr);
    void setupConnections(QObject *mainWindow);

private slots:
    void onCheckPythonVersionClicked();
    void onCheckWeb3Clicked();
    void onCheckBalanceClicked();
    void onCheckDockerInstallClicked();

private:
    Ui::MainWindow *ui;
    QLabel *pythonLabel;
    QLabel *addressLabel;
    QLabel *balanceLabel;
    QLabel *web3StatusLabel;
    QLabel *dockerInstallLabel;
};

#endif // SETTINGSPAGE_H
