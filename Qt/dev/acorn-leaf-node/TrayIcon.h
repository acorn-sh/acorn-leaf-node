#ifndef TRAYICON_H
#define TRAYICON_H

#include <QObject>
#include <QSystemTrayIcon>
#include <QMenu>
#include "ResourceManager.h"

class TrayIcon : public QObject
{
    Q_OBJECT

public:
    explicit TrayIcon(ResourceManager* resourceManager, QObject *parent = nullptr);
    ~TrayIcon();

    void show();
    void hide();
    void showMessage(const QString &title, const QString &message, QSystemTrayIcon::MessageIcon icon, int msecs);

signals:
    void restoreRequested();
    void quitRequested();

private slots:
    void onActivated(QSystemTrayIcon::ActivationReason reason);

private:
    QSystemTrayIcon *trayIcon;
    QMenu *trayMenu;
    QAction *restoreAction;
    QAction *quitAction;
};

#endif // TRAYICON_H
