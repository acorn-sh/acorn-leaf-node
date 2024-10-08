#ifndef PAYMENTMANAGER_H
#define PAYMENTMANAGER_H

#include <QObject>
#include <QString>
#include "BlockchainManager.h"

class PaymentManager : public QObject {
    Q_OBJECT

public:
    PaymentManager(BlockchainManager* blockchainManager, QObject* parent = nullptr);

    void calculateAndTriggerPayment(const QString& containerId, int uptimeSeconds, double cpuUsage, double gpuUsage, double memoryUsage, double networkUsage);

signals:
    void paymentTriggered(const QString& containerId, double amount);
    void logMessage(const QString& message);

private:
    BlockchainManager* blockchainManager;

    double calculatePayment(int uptimeSeconds, double cpuUsage, double gpuUsage, double memoryUsage, double networkUsage);
};

#endif // PAYMENTMANAGER_H
