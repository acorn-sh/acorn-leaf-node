#ifndef HUBPAGE_H
#define HUBPAGE_H

#include "ui_MainWindow.h"
#include "ResourceManager.h"
#include "Terminal.h"

#include <QObject>
#include <QProgressBar>
#include <QProcess>
#include <QStringList>

class HubPage : public QObject
{
    Q_OBJECT

public:
    explicit HubPage(Ui::MainWindow *ui, ResourceManager *resourceManager, QObject *parent = nullptr);
    ~HubPage();

    void setupConnections(QObject *mainWindow);
    void populateHubTable();
    QStringList getDockerHubRepos();

private slots:
    void handleInstall(int row);
    void handleStart(int row);
    void handleDelete(int row);

private:
    Ui::MainWindow *ui;
    QMap<int, QProcess*> activeProcesses;
    ResourceManager *resourceManager;
    Terminal *terminal;

    void setupTable();
    void adjustColumnWidths();
    void addControlButtons(int row);
    QPushButton* createControlButton(const QString &iconName, const QString &action, int row);
    void replaceButtonsWithProgressBar(int row);
    QString fetchImageID(const QString &repoName);  // Change return type to QString
};

#endif // HUBPAGE_H
