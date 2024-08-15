#ifndef HELPPAGE_H
#define HELPPAGE_H

#include <QDialog>
#include "ui_mainwindow.h"
#include <QCheckBox>
#include <QSettings>

class HelpPage : public QDialog
{
    Q_OBJECT

public:
    explicit HelpPage(QWidget *parent = nullptr);
    void populateHelpContent();
    static bool shouldShowHelp();  // Checks if the HelpPage should be shown

private slots:
    void onCheckboxChanged(int state);

private:
    Ui::MainWindow *ui;
    QCheckBox *doNotShowAgainCheckbox;
    void setupUI();
};

#endif // HELPPAGE_H


