#include "helppage.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QFrame>
#include <QPushButton>
#include <QCheckBox>
#include <QHBoxLayout>

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

    // Calculate the fixed height for the frames
    int helpPageHeight = this->height();
    int bottomLayoutHeight = 50; // Approximate height for the checkbox and close button layout
    int availableHeightForFrames = helpPageHeight - bottomLayoutHeight - 20; // 20px for padding
    int frameHeight = availableHeightForFrames;

    // Create three frames equally spaced horizontally
    QHBoxLayout *hLayout = new QHBoxLayout();

    // Define content variables
    QStringList images = {":/images/feature1.png", ":/images/feature2.png", ":/images/feature3.png"};
    QStringList descriptions = {
        "Install dependencies.",
        "Download docker containers.",
        "Run containers and earn Acorn tokens."
    };

    int frameWidth = (this->width() - 40) / 3; // Calculate the width for each frame

    for (int i = 0; i < 3; ++i) {
        QFrame *frame = new QFrame(this);
        frame->setFixedSize(frameWidth, frameHeight); // Set fixed size for each frame
        frame->setStyleSheet("background-color: #ffffff; border: 1px solid #ccc;"); // Border only for the frame
        QVBoxLayout *frameLayout = new QVBoxLayout(frame);

        QLabel *imageLabel = new QLabel(frame);
        imageLabel->setPixmap(QPixmap(images[i]).scaled(frameWidth - 20, frameHeight - 50, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        imageLabel->setStyleSheet("border: none;");  // Remove border from the image
        frameLayout->addWidget(imageLabel, 0, Qt::AlignCenter);

        QLabel *textLabel = new QLabel(descriptions[i], frame);
        textLabel->setWordWrap(true);
        textLabel->setAlignment(Qt::AlignCenter); // Center the text
        textLabel->setStyleSheet("border: none;");  // Remove border from the text
        frameLayout->addWidget(textLabel, 0, Qt::AlignCenter);

        hLayout->addWidget(frame);
        hLayout->setStretch(i, 1);  // Make frames expandable
    }

    layout->addLayout(hLayout);
    layout->addStretch();  // Add stretch to push the bottom layout down

    // Create a horizontal layout for the checkbox and close button
    QHBoxLayout *bottomLayout = new QHBoxLayout();

    doNotShowAgainCheckbox = new QCheckBox("Do not show again", this);
    QPushButton *closeButton = new QPushButton("Close", this);

    // Center the checkbox and close button in the layout
    bottomLayout->addStretch(); // Add stretch before the items
    bottomLayout->addWidget(doNotShowAgainCheckbox, 0, Qt::AlignCenter);
    bottomLayout->addWidget(closeButton, 0, Qt::AlignCenter);
    bottomLayout->addStretch(); // Add stretch after the items

    layout->addLayout(bottomLayout);

    // Set the background of the popup
    this->setStyleSheet("background-color: rgba(255, 255, 255, 240);");

    this->setLayout(layout);

    // Connect the close button to close the dialog
    connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);

    // Connect the checkbox state change to the slot
    connect(doNotShowAgainCheckbox, &QCheckBox::stateChanged, this, &HelpPage::onCheckboxChanged);
}

void HelpPage::populateHelpContent()
{
    // Content population logic is already handled in setupUI
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
