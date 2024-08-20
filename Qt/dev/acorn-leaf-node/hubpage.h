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
    void fetchDockerHubRepos();  // Slot to fetch repositories

private:
    Ui::MainWindow *ui;
    QMap<int, QProcess*> activeProcesses;  // To track running processes by row

    void setupTable();  // Declare setupTable method
    void adjustColumnWidths();  // Declare adjustColumnWidths method
    void addControlButtons(int row);  // Declare addControlButtons method
    void handleInstall(int row);  // Declare handleInstall method
    void handleStatus(int row);  // Declare handleStatus method
    void updateInstallButton(int row);  // Update the install button to check icon
    void displayLogInTerminal(const QString &log);  // Display logs in the terminal
};

#endif // HUBPAGE_H
