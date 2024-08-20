#ifndef ACCOUNTPAGE_H
#define ACCOUNTPAGE_H

#include <QObject>
#include <QLabel>
#include <QPushButton>
#include "ui_mainwindow.h"

class AccountPage : public QObject
{
    Q_OBJECT

public:
    explicit AccountPage(Ui::MainWindow *ui, QObject *parent = nullptr);
    void setupConnections();

    void refreshEthereumAddress();

private slots:
    void onGenerateAddressButtonClicked();

private:
    Ui::MainWindow *ui;
    QLabel *accountLabel;
    QLabel *privateKeyLabel;
    QPushButton *generateAddressButton;

    void displayEthereumAddress(const QString &address, const QString &privateKey = QString());
    void generateNewEthereumAddress();
};

#endif // ACCOUNTPAGE_H
