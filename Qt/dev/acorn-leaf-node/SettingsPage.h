#ifndef SETTINGSPAGE_H
#define SETTINGSPAGE_H

#include "ui_MainWindow.h"
#include "ResourceManager.h"
#include <QLabel>
#include <QObject>

class SettingsPage : public QObject
{
    Q_OBJECT

public:
    explicit SettingsPage(Ui::MainWindow *ui, ResourceManager *resourceManager, QObject *parent = nullptr);
    ~SettingsPage();
    void setupConnections(QObject *mainWindow);

private slots:
    void onCheckPythonVersionClicked();
    void onCheckWeb3Clicked();
    void onCheckContractClicked();
    void onCheckDockerInstallClicked();

private:
    Ui::MainWindow *ui;
    ResourceManager *resourceManager;

    QLabel *pythonLabel;
    QLabel *contractAddressLabel;
    QLabel *contractBalanceLabel;
    QLabel *web3StatusLabel;
    QLabel *dockerInstallLabel;
};

#endif // SETTINGSPAGE_H
