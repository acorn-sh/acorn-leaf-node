#ifndef HUBTABLE_H
#define HUBTABLE_H

#include <QObject>
#include <QTableWidget>
#include "ui_MainWindow.h"
#include "Components.h"  // Include Components
#include "ResourceManager.h"  // Include ResourceManager

class HubTable : public QObject
{
    Q_OBJECT

public:
    explicit HubTable(Ui::MainWindow* ui, ResourceManager* resourceManager, QObject* parent = nullptr);

    void populateHubTable(const QStringList &repos);
    void addControlButtons(int row);
    void replaceButtonsWithProgressBar(int row);
    QString getImageIDs(const QString &repoName);

signals:
    void installRequested(int row);
    void startRequested(int row);
    void deleteRequested(int row);

private:
    void setupTable();

    Ui::MainWindow* ui;
    ResourceManager* resourceManager;  // Declare ResourceManager
    Components* components;  // Declare Components
};

#endif // HUBTABLE_H
