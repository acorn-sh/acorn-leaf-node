#ifndef HUBPAGE_H
#define HUBPAGE_H

#include <QObject>
#include <QProgressBar>
#include <QProcess>
#include "ui_mainwindow.h"

class HubPage : public QObject
{
    Q_OBJECT

public:
    explicit HubPage(Ui::MainWindow *ui, QObject *parent = nullptr);
    void setupConnections(QObject *mainWindow);
    void populateHubTable();

private slots:
    void fetchDockerHubRepos();

private:
    Ui::MainWindow *ui;
    QMap<int, QProcess*> activeProcesses;

    void setupTable();
    void adjustColumnWidths();
    void addControlButtons(int row);
    void handleInstall(int row);
    void handleStatus(int row);
    void updateInstallButton(int row);
    void displayLogInTerminal(const QString &log);
};

#endif // HUBPAGE_H
