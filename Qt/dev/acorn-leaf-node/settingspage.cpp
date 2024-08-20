#include "settingspage.h"
#include <QProcess>
#include <QDebug>
#include <QMessageBox>
#include <QCoreApplication>

SettingsPage::SettingsPage(Ui::MainWindow *ui, QObject *parent)
    : QObject(parent), ui(ui)
{
    pythonLabel = ui->pythonLabel;
    contractAddressLabel = ui->contractAddressLabel;
    contractBalanceLabel = ui->contractBalanceLabel;
    web3StatusLabel = ui->web3StatusLabel;
    dockerInstallLabel = ui->dockerInstallLabel;
}

void SettingsPage::setupConnections(QObject *mainWindow)
{
    connect(ui->checkPythonButton, &QPushButton::clicked, this, &SettingsPage::onCheckPythonVersionClicked);
    connect(ui->checkWeb3Button, &QPushButton::clicked, this, &SettingsPage::onCheckWeb3Clicked);
    connect(ui->checkContractButton, &QPushButton::clicked, this, &SettingsPage::onCheckContractClicked);
    connect(ui->checkDockerInstallButton, &QPushButton::clicked, this, &SettingsPage::onCheckDockerInstallClicked);
}

void SettingsPage::onCheckPythonVersionClicked()
{
    QProcess *process = new QProcess(this);
    connect(process, &QProcess::readyReadStandardOutput, this, [process, this]() {
        QByteArray output = process->readAllStandardOutput();
        QString version = QString(output).trimmed();
        pythonLabel->setText("Python Version: " + version);
    });

    QString pythonPath = QCoreApplication::applicationDirPath() + "/bundled_python/bin/python3.11";
    process->start(pythonPath, QStringList() << QCoreApplication::applicationDirPath() + "/check_python.py");
}

void SettingsPage::onCheckWeb3Clicked()
{
    QProcess *process = new QProcess(this);
    connect(process, &QProcess::readyReadStandardOutput, this, [process, this]() {
        QByteArray output = process->readAllStandardOutput();
        QString status = QString(output).trimmed();
        web3StatusLabel->setText("Web3 Status: " + status);
    });

    QString pythonPath = QCoreApplication::applicationDirPath() + "/bundled_python/bin/python3.11";
    process->start(pythonPath, QStringList() << QCoreApplication::applicationDirPath() + "/check_web3py.py");
}

void SettingsPage::onCheckContractClicked()
{
    QString address = "0xDf6e59c6DF1E9500fd35A76FF4C62F9901E90019";
    contractAddressLabel->setText("Ethereum Address: " + address);

    QProcess *process = new QProcess(this);
    connect(process, &QProcess::readyReadStandardOutput, this, [process, this]() {
        QByteArray output = process->readAllStandardOutput();
        QString balance = QString(output).trimmed();
        if (balance.isEmpty()) {
            contractBalanceLabel->setText("Failed to retrieve balance");
        } else {
            contractBalanceLabel->setText(balance);
        }
    });
    connect(process, &QProcess::readyReadStandardError, this, [process, this]() {
        QByteArray errorOutput = process->readAllStandardError();
        QString errorString = QString(errorOutput).trimmed();
        qDebug() << "Error:" << errorString;
        QMessageBox::critical(nullptr, "Error", "Failed to retrieve balance:\n" + errorString);
    });

    QString pythonPath = QCoreApplication::applicationDirPath() + "/bundled_python/bin/python3.11";
    process->start(pythonPath, QStringList() << QCoreApplication::applicationDirPath() + "/check_account.py" << address);
}

void SettingsPage::onCheckDockerInstallClicked()
{
    QProcess *process = new QProcess(this);

    // Set up the environment
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    QString path = env.value("PATH");

#ifdef Q_OS_WIN
    // Windows-specific path adjustment
    QString dockerPath = "C:\\Program Files\\Docker\\Docker\\resources\\bin";
    if (!path.contains(dockerPath))
    {
        path = dockerPath + ";" + path;
        env.insert("PATH", path);
    }
#else
    // macOS/Linux-specific path adjustment
    if (!path.contains("/usr/local/bin"))
    {
        path = "/usr/local/bin:" + path;
        env.insert("PATH", path);
    }
#endif

    process->setProcessEnvironment(env);

    // Connect to standard output
    connect(process, &QProcess::readyReadStandardOutput, this, [process, this]() {
        QByteArray output = process->readAllStandardOutput();
        QString message = QString(output).trimmed();
        dockerInstallLabel->setText(message);
    });

    // Connect to standard error
    connect(process, &QProcess::readyReadStandardError, this, [process, this]() {
        QByteArray errorOutput = process->readAllStandardError();
        QString errorMessage = QString(errorOutput).trimmed();
        qDebug() << "Error:" << errorMessage;
        QMessageBox::critical(nullptr, "Docker Check Error", errorMessage);
    });

    // Start the Python script
    QString pythonPath = QCoreApplication::applicationDirPath() + "/bundled_python/bin/python3.11";
    QString scriptPath = QCoreApplication::applicationDirPath() + "/check_docker.py";
    process->start(pythonPath, QStringList() << scriptPath);
}

