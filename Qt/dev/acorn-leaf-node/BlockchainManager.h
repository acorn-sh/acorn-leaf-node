#ifndef BLOCKCHAINMANAGER_H
#define BLOCKCHAINMANAGER_H

#include <QObject>
#include <QString>
#include "ResourceManager.h"

class BlockchainManager : public QObject {
    Q_OBJECT

public:
    explicit BlockchainManager(ResourceManager* resourceManager, QObject* parent = nullptr);

    QString generateNewAddress();
    QString getAccountBalance(const QString& address);
    bool sendTransaction(const QString& fromAddress, const QString& toAddress, double amount);

signals:
    void transactionSent(const QString& transactionHash);
    void logMessage(const QString& message);

private:
    ResourceManager* resourceManager;
};

#endif // BLOCKCHAINMANAGER_H
