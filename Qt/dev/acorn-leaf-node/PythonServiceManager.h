#ifndef PYTHONSERVICEMANAGER_H
#define PYTHONSERVICEMANAGER_H

#include "ResourceManager.h"

#include <QObject>
#include <QProcess>
#include <QStringList>
#include <QMap>

class PythonServiceManager : public QObject {
    Q_OBJECT

public:
    explicit PythonServiceManager(ResourceManager* resourceManager, QObject* parent = nullptr);

    void runScript(const QString& scriptName, const QStringList& arguments = QStringList());
    void startService(const QString& serviceName, const QStringList& arguments = QStringList());
    void stopService(const QString& serviceName);
    bool isServiceRunning(const QString& serviceName) const;

signals:
    void scriptOutputReceived(const QString& scriptName, const QString& output);
    void serviceStarted(const QString& serviceName);
    void serviceStopped(const QString& serviceName);
    void logMessage(const QString& message);

private:
    QString pythonPath;
    QMap<QString, QProcess*> runningServices;

    void handleProcessOutput(QProcess* process, const QString& scriptName);
    void handleProcessError(QProcess* process, const QString& scriptName);
};

#endif // PYTHONSERVICEMANAGER_H
