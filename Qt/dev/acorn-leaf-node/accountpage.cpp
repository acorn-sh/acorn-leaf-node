#include "accountpage.h"
#include <QProcess>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QCoreApplication>
#include <QDebug>
#include <QMessageBox>

AccountPage::AccountPage(Ui::MainWindow *ui, QObject *parent)
    : QObject(parent), ui(ui)
{
    // Initialize UI elements
    accountLabel = ui->accountLabel;
    privateKeyLabel = ui->privateKeyLabel;  // Assuming you have a QLabel for the private key
    generateAddressButton = ui->generateAddressButton;

    // Set up the button click connection
    setupConnections();
}

void AccountPage::setupConnections()
{
    connect(generateAddressButton, &QPushButton::clicked, this, &AccountPage::onGenerateAddressButtonClicked);
}

void AccountPage::onGenerateAddressButtonClicked()
{
    generateNewEthereumAddress();
}

void AccountPage::generateNewEthereumAddress()
{
    QString pythonPath = QCoreApplication::applicationDirPath() + "/bundled_python/bin/python3.11";
    QString scriptPath = QCoreApplication::applicationDirPath() + "/generate_work_address.py";

    QProcess *process = new QProcess(this);
    connect(process, &QProcess::readyReadStandardOutput, this, [process, this]() {
        QByteArray output = process->readAllStandardOutput();
        QString outputString = QString(output).trimmed();
        qDebug() << "Python output:" << outputString;

        // Parse the JSON output to extract the address and private key
        QJsonDocument doc = QJsonDocument::fromJson(output);
        QJsonObject obj = doc.object();
        QString address = obj.value("address").toString();
        QString privateKey = obj.value("private_key").toString();

        displayEthereumAddress(address, privateKey);
    });

    connect(process, &QProcess::readyReadStandardError, this, [process, this]() {
        QByteArray errorOutput = process->readAllStandardError();
        QString errorMessage = QString(errorOutput).trimmed();
        qDebug() << "Error:" << errorMessage;
        QMessageBox::critical(nullptr, "Error", "Failed to generate new Ethereum address:\n" + errorMessage);
    });

    process->start(pythonPath, QStringList() << scriptPath);
}

void AccountPage::displayEthereumAddress(const QString &address, const QString &privateKey)
{
    if (!address.isEmpty()) {
        accountLabel->setText("Ethereum Address: " + address);
    } else {
        accountLabel->setText("No Ethereum Address");
    }

    if (!privateKey.isEmpty()) {
        privateKeyLabel->setText("Private Key: " + privateKey);
    } else {
        privateKeyLabel->setText("No Private Key");
    }
}

// Public method to refresh the Ethereum address
void AccountPage::refreshEthereumAddress()
{
    QString appDirPath = QCoreApplication::applicationDirPath();
    QString filePath = appDirPath + "/eth_account.json";

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open Ethereum account file for reading.";
        accountLabel->setText("No Ethereum Address");
        privateKeyLabel->setText("No Private Key");
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonObject obj = doc.object();
    QString address = obj.value("address").toString().trimmed();
    QString privateKey = obj.value("private_key").toString().trimmed();

    if (!address.isEmpty() && !privateKey.isEmpty()) {
        displayEthereumAddress(address, privateKey);
    } else {
        accountLabel->setText("No Ethereum Address");
        privateKeyLabel->setText("No Private Key");
    }
}
