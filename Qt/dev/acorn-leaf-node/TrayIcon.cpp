#include "TrayIcon.h"
#include <QApplication>

TrayIcon::TrayIcon(ResourceManager* resourceManager, QObject *parent)
    : QObject(parent), trayIcon(new QSystemTrayIcon(this)), trayMenu(new QMenu())
{
    QIcon icon = resourceManager->getIcon("acorn");
    trayIcon->setIcon(icon);
    trayIcon->setToolTip("Acorn SH");

    // Create tray menu actions
    restoreAction = trayMenu->addAction("Restore");
    quitAction = trayMenu->addAction("Quit");

    connect(restoreAction, &QAction::triggered, this, &TrayIcon::restoreRequested);
    connect(quitAction, &QAction::triggered, this, &TrayIcon::quitRequested);

    trayIcon->setContextMenu(trayMenu);

    connect(trayIcon, &QSystemTrayIcon::activated, this, &TrayIcon::onActivated);
}

TrayIcon::~TrayIcon()
{
    delete trayMenu;
}

void TrayIcon::show()
{
    trayIcon->show();
}

void TrayIcon::hide()
{
    trayIcon->hide();
}

void TrayIcon::showMessage(const QString &title, const QString &message, QSystemTrayIcon::MessageIcon icon, int msecs)
{
    trayIcon->showMessage(title, message, icon, msecs);
}

void TrayIcon::onActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::Trigger) {
        emit restoreRequested();
    }}
