#include "PaymentManager.h"
#include <QDebug>

PaymentManager::PaymentManager(BlockchainManager* blockchainManager, QObject* parent)
    : QObject(parent), blockchainManager(blockchainManager) {}

void PaymentManager::calculateAndTriggerPayment(const QString& containerId, int uptimeSeconds, double cpuUsage, double gpuUsage, double memoryUsage, double networkUsage) {
    double paymentAmount = calculatePayment(uptimeSeconds, cpuUsage, gpuUsage, memoryUsage, networkUsage);

    // For demonstration, simulate the "from" and "to" Ethereum addresses.
    QString fromAddress = "0xYourEthereumAddressHere";
    QString toAddress = "0xRecipientEthereumAddressHere";

    bool success = blockchainManager->sendTransaction(fromAddress, toAddress, paymentAmount);

    if (success) {
        emit paymentTriggered(containerId, paymentAmount);
        emit logMessage(QString("Payment of %1 tokens triggered for container %2").arg(paymentAmount).arg(containerId));
    } else {
        emit logMessage(QString("Payment failed for container %1").arg(containerId));
    }
}

double PaymentManager::calculatePayment(int uptimeSeconds, double cpuUsage, double gpuUsage, double memoryUsage, double networkUsage) {
    // Example coefficients for each resource type
    double cpuCoefficient = 1.2;
    double gpuCoefficient = 1.5;
    double memoryCoefficient = 1.0;
    double networkCoefficient = 0.8;

    // Basic rate per second of uptime (this is just an example)
    double baseRate = 0.01;

    // Calculate payment amount based on uptime and resource usage
    double paymentAmount = baseRate * uptimeSeconds
                           + cpuCoefficient * cpuUsage
                           + gpuCoefficient * gpuUsage
                           + memoryCoefficient * memoryUsage
                           + networkCoefficient * networkUsage;

    return paymentAmount;
}
