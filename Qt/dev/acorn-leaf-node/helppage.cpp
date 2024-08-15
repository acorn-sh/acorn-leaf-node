#include "helppage.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QTableWidget>
#include <QHeaderView>
#include <QPushButton>

HelpPage::HelpPage(QWidget *parent)
    : QDialog(parent, Qt::Dialog | Qt::FramelessWindowHint | Qt::WindowSystemMenuHint)
{
    setupUI();
    populateHelpContent();
}

void HelpPage::setupUI()
{
    // Set the size of the HelpPage to 80% of the main window size
    this->resize(parentWidget()->width() * 0.8, parentWidget()->height() * 0.8);
    this->setWindowTitle("Help Page");

    // Position the help page in the center of the parent window
    move(parentWidget()->geometry().center() - rect().center());

    QVBoxLayout *layout = new QVBoxLayout(this);

    // Create a table widget with three columns
    QTableWidget *helpTable = new QTableWidget(this);
    helpTable->setColumnCount(3);

    // Set the header labels for the columns
    QStringList headers = {"Feature 1", "Feature 2", "Feature 3"};
    helpTable->setHorizontalHeaderLabels(headers);
    helpTable->horizontalHeader()->setStretchLastSection(true); // Stretch the last section to fit

    // Customize the appearance
    helpTable->setStyleSheet("background-color: #ffffff; color: black; font-size: 14px;");
    helpTable->setEditTriggers(QAbstractItemView::NoEditTriggers);  // Make table non-editable
    helpTable->setSelectionMode(QAbstractItemView::NoSelection);    // Disable selection
    helpTable->setShowGrid(false);                                  // Hide grid lines

    layout->addWidget(helpTable);

    // Add the "Do not show again" checkbox and Close button
    doNotShowAgainCheckbox = new QCheckBox("Do not show again", this);
    QPushButton *closeButton = new QPushButton("Close", this);

    QHBoxLayout *bottomLayout = new QHBoxLayout();
    bottomLayout->addWidget(doNotShowAgainCheckbox, 0, Qt::AlignLeft);
    bottomLayout->addWidget(closeButton, 0, Qt::AlignRight);

    connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(doNotShowAgainCheckbox, &QCheckBox::stateChanged, this, &HelpPage::onCheckboxChanged);

    layout->addLayout(bottomLayout);

    this->setLayout(layout);

    // Set the background of the popup
    this->setStyleSheet("background-color: rgba(255, 255, 255, 240);");
}



void HelpPage::populateHelpContent()
{
    // Example of adding content to the help table
    QTableWidget *helpTable = this->findChild<QTableWidget *>();

    if (!helpTable) return;

    helpTable->setRowCount(3);  // Example: 3 rows for 3 features

    // Example content for each feature
    for (int i = 0; i < 3; ++i) {
        QLabel *imageLabel = new QLabel();
        imageLabel->setPixmap(QPixmap(":/images/feature" + QString::number(i + 1) + ".png").scaled(100, 100, Qt::KeepAspectRatio));
        helpTable->setCellWidget(i, 0, imageLabel);

        QLabel *textLabel = new QLabel("Description of Feature " + QString::number(i + 1));
        textLabel->setWordWrap(true);
        helpTable->setCellWidget(i, 1, textLabel);
    }

    helpTable->resizeColumnsToContents();  // Adjust the column width based on content
    helpTable->resizeRowsToContents();     // Adjust the row height based on content
}

void HelpPage::onCheckboxChanged(int state)
{
    QSettings settings;
    settings.setValue("helpPage/doNotShowAgain", state == Qt::Checked);
}

bool HelpPage::shouldShowHelp()
{
    QSettings settings;
    return !settings.value("helpPage/doNotShowAgain", false).toBool();
}
