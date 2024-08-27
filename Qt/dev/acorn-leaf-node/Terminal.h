#ifndef TERMINAL_H
#define TERMINAL_H

#include <QObject>
#include <QString>

class QPlainTextEdit;  // Forward declare QPlainTextEdit
class QPushButton;
class ResourceManager;

class Terminal : public QObject
{
    Q_OBJECT

public:
    Terminal(QPlainTextEdit *terminalWidget, QPushButton *expandButton, QObject *parent = nullptr);
    void setupTerminal();
    void appendLog(const QString &log);
    void toggleExpansion(ResourceManager *resourceManager);

private:
    void scrollToBottom();

    QPlainTextEdit *terminalWidget;  // The specific terminal widget
    QPushButton *expandButton;       // The expand button widget
};

#endif // TERMINAL_H
