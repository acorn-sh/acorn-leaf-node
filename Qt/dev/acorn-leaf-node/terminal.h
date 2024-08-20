#ifndef TERMINAL_H
#define TERMINAL_H

#include <QObject>
#include "ui_mainwindow.h"

class Terminal : public QObject
{
    Q_OBJECT

public:
    Terminal(Ui::MainWindow *ui);
    void setupTerminal();
    void scrollToBottom();

private:
    Ui::MainWindow *ui;
};

#endif // TERMINAL_H
