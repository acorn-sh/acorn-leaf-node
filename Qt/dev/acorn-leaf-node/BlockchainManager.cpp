#include "BlockchainManager.h"
#include <QProcess>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

BlockchainManager::BlockchainManager(ResourceManager* resourceManager, QObject* parent)
    : QObject(parent), resourceManager(resourceManager) {}

QString BlockchainManager::generateNewAddress() {
    QString pythonPath = resourceManager->getPythonPath();
    QString scriptPath = resourceManager->getScriptPath("generate_work_address.py");

    QProcess process;
    process.start(pythonPath, QStringList() << scriptPath);
    process.waitForFinished();

    QByteArray output = process.readAllStandardOutput();
    QJsonDocument doc = QJsonDocument::fromJson(output);
    QJsonObject obj = doc.object();
    QString address = obj.value("address").toString();
    QString privateKey = obj.value("private_key").toString();

    emit logMessage(QString("Generated new Ethereum address: %1").arg(address));
    return address;
}

QString BlockchainManager::getAccountBalance(const QString& address) {
    QString pythonPath = resourceManager->getPythonPath();
    QString scriptPath = resourceManager->getScriptPath("check_balance.py");

    QProcess process;
    process.start(pythonPath, QStringList() << scriptPath << address);
    process.waitForFinished();

    QString balance = process.readAllStandardOutput().trimmed();
    emit logMessage(QString("Balance for address %1: %2").arg(address).arg(balance));
    return balance;
}

bool BlockchainManager::sendTransaction(const QString& fromAddress, const QString& toAddress, double amount) {
    QString pythonPath = resourceManager->getPythonPath();
    QString scriptPath = resourceManager->getScriptPath("send_transaction.py");

    QProcess process;
    process.start(pythonPath, QStringList() << scriptPath << fromAddress << toAddress << QString::number(amount));
    process.waitForFinished();

    QByteArray output = process.readAllStandardOutput();
    QJsonDocument doc = QJsonDocument::fromJson(output);
    QJsonObject obj = doc.object();
    QString txHash = obj.value("transaction_hash").toString();

    if (!txHash.isEmpty()) {
        emit transactionSent(txHash);
        emit logMessage(QString("Transaction sent with hash: %1").arg(txHash));
        return true;
    } else {
        emit logMessage("Failed to send transaction");
        return false;
    }
}
