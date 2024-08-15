#ifndef HUBPAGE_H
#define HUBPAGE_H

#include <QObject>
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

    void setupTable();  // Declare setupTable method
    void adjustColumnWidths();  // Declare adjustColumnWidths method
    void addControlButtons(int row);  // Declare addControlButtons method
    void handleInstall(int row);  // Declare handleInstall method
    void handleStatus(int row);  // Declare handleStatus method
};

#endif // HUBPAGE_H
