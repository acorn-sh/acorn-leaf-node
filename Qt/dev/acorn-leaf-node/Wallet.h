// Wallet.h
#ifndef WALLET_H
#define WALLET_H

#include <QObject>
#include <QString>
#include <QProcess>
#include "ResourceManager.h"

class Wallet : public QObject
{
    Q_OBJECT

public:
    Wallet(ResourceManager* resourceManager, QObject* parent = nullptr);
    ~Wallet();

    void checkBalance(const QString& address);
    void sendTransaction(const QString& fromAddress, const QString& toAddress, const QString& amount);
    void interactWithContract(const QString& contractAddress, const QString& method, const QStringList& params);

signals:
    void balanceChecked(const QString& balance);
    void transactionSent(const QString& result);
    void contractInteracted(const QString& result);
    void errorOccurred(const QString& errorMessage);

private:
    ResourceManager* resourceManager;

    void runPythonScript(const QString& scriptName, const QStringList& arguments);
};

#endif // WALLET_H
