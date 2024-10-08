// Wallet.cpp
#include "Wallet.h"
#include <QMessageBox>
#include <QDebug>

Wallet::Wallet(ResourceManager* resourceManager, QObject* parent)
    : QObject(parent), resourceManager(resourceManager)
{
}

Wallet::~Wallet() {
    // No need to delete resourceManager since it is managed elsewhere
}

void Wallet::checkBalance(const QString& address) {
    QStringList arguments;
    arguments << address;
    runPythonScript("check_account.py", arguments);
}

void Wallet::sendTransaction(const QString& fromAddress, const QString& toAddress, const QString& amount) {
    QStringList arguments;
    arguments << fromAddress << toAddress << amount;
    runPythonScript("send_transaction.py", arguments);
}

void Wallet::interactWithContract(const QString& contractAddress, const QString& method, const QStringList& params) {
    QStringList arguments;
    arguments << contractAddress << method << params;
    runPythonScript("interact_contract.py", arguments);
}

void Wallet::runPythonScript(const QString& scriptName, const QStringList& arguments) {
    QString pythonPath = resourceManager->getPythonPath();
    QString scriptPath = resourceManager->getScriptPath(scriptName);

    QProcess* process = new QProcess(this);
    process->setProcessEnvironment(resourceManager->getProcessEnvironment());

    connect(process, &QProcess::readyReadStandardOutput, this, [this, process]() {
        QString output = process->readAllStandardOutput().trimmed();
        emit balanceChecked(output);  // Adapt this based on the method calling it
    });

    connect(process, &QProcess::readyReadStandardError, this, [this, process]() {
        QString errorOutput = process->readAllStandardError().trimmed();
        emit errorOccurred("Error: " + errorOutput);
    });

    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, [this, process]() {
        process->deleteLater();  // Clean up the process
    });

    process->start(pythonPath, QStringList() << scriptPath << arguments);
}
