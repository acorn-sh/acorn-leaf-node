#include "PythonServiceManager.h"
#include "ResourceManager.h"

#include <QDebug>

PythonServiceManager::PythonServiceManager(ResourceManager* resourceManager, QObject* parent)
    : QObject(parent), pythonPath(resourceManager->getPythonPath()) {

}

void PythonServiceManager::runScript(const QString& scriptName, const QStringList& arguments) {
    QStringList args;
    args << scriptName << arguments;

    QProcess* process = new QProcess(this);
    connect(process, &QProcess::readyReadStandardOutput, this, [this, process, scriptName]() {
        handleProcessOutput(process, scriptName);
    });
    connect(process, &QProcess::readyReadStandardError, this, [this, process, scriptName]() {
        handleProcessError(process, scriptName);
    });
    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, [this, process, scriptName](int exitCode, QProcess::ExitStatus status) {
        qDebug() << "Script" << scriptName << "finished with exit code" << exitCode;
        emit logMessage(QString("Script %1 finished with exit code %2").arg(scriptName).arg(exitCode));
        process->deleteLater();
    });

    process->start(pythonPath, args);
}

void PythonServiceManager::startService(const QString& serviceName, const QStringList& arguments) {
    if (runningServices.contains(serviceName)) {
        emit logMessage(QString("Service %1 is already running").arg(serviceName));
        return;
    }

    QStringList args;
    args << serviceName << arguments;

    QProcess* process = new QProcess(this);
    runningServices.insert(serviceName, process);

    connect(process, &QProcess::readyReadStandardOutput, this, [this, process, serviceName]() {
        handleProcessOutput(process, serviceName);
    });
    connect(process, &QProcess::readyReadStandardError, this, [this, process, serviceName]() {
        handleProcessError(process, serviceName);
    });
    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, [this, serviceName, process](int exitCode, QProcess::ExitStatus status) {
        emit logMessage(QString("Service %1 stopped with exit code %2").arg(serviceName).arg(exitCode));
        emit serviceStopped(serviceName);
        runningServices.remove(serviceName);
        process->deleteLater();
    });

    process->start(pythonPath, args);
    emit serviceStarted(serviceName);
}

void PythonServiceManager::stopService(const QString& serviceName) {
    if (!runningServices.contains(serviceName)) {
        emit logMessage(QString("Service %1 is not running").arg(serviceName));
        return;
    }

    QProcess* process = runningServices.value(serviceName);
    process->terminate();
    process->waitForFinished(3000);  // Wait for 3 seconds to terminate

    if (process->state() != QProcess::NotRunning) {
        process->kill();  // Force kill if not stopped
    }
}

bool PythonServiceManager::isServiceRunning(const QString& serviceName) const {
    return runningServices.contains(serviceName);
}

void PythonServiceManager::handleProcessOutput(QProcess* process, const QString& scriptName) {
    QString output = process->readAllStandardOutput().trimmed();
    emit scriptOutputReceived(scriptName, output);
    emit logMessage(output);
}

void PythonServiceManager::handleProcessError(QProcess* process, const QString& scriptName) {
    QString errorOutput = process->readAllStandardError().trimmed();
    emit logMessage(QString("Error in script %1: %2").arg(scriptName).arg(errorOutput));
}
