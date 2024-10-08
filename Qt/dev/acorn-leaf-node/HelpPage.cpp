#include "HelpPage.h"
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
    this->resize(parentWidget()->width() * 0.8, parentWidget()->height() * 0.8);
    this->setWindowTitle("Help Page");

    move(parentWidget()->geometry().center() - rect().center());

    QVBoxLayout *layout = new QVBoxLayout(this);

    int helpPageHeight = this->height();
    int bottomLayoutHeight = 50;
    int availableHeightForFrames = helpPageHeight - bottomLayoutHeight - 20;
    int frameHeight = availableHeightForFrames;

    QHBoxLayout *hLayout = new QHBoxLayout();

    QStringList images = {":/images/feature1.png", ":/images/feature2.png", ":/images/feature3.png"};
    QStringList descriptions = {
        "Install dependencies.",
        "Download docker containers.",
        "Run containers and earn Acorn tokens."
    };

    int frameWidth = (this->width() - 40) / 3;

    for (int i = 0; i < 3; ++i) {
        QFrame *frame = new QFrame(this);
        frame->setFixedSize(frameWidth, frameHeight);
        frame->setStyleSheet("background-color: #ffffff; border: 1px solid #ccc;");
        QVBoxLayout *frameLayout = new QVBoxLayout(frame);

        QLabel *imageLabel = new QLabel(frame);
        imageLabel->setPixmap(QPixmap(images[i]).scaled(frameWidth - 20, frameHeight - 50, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        imageLabel->setStyleSheet("border: none;");
        frameLayout->addWidget(imageLabel, 0, Qt::AlignCenter);

        QLabel *textLabel = new QLabel(descriptions[i], frame);
        textLabel->setWordWrap(true);
        textLabel->setAlignment(Qt::AlignCenter);
        textLabel->setStyleSheet("border: none;");
        frameLayout->addWidget(textLabel, 0, Qt::AlignCenter);

        hLayout->addWidget(frame);
        hLayout->setStretch(i, 1);
    }

    layout->addLayout(hLayout);
    layout->addStretch();

    QHBoxLayout *bottomLayout = new QHBoxLayout();

    doNotShowAgainCheckbox = new QCheckBox("Do not show again", this);
    QPushButton *closeButton = new QPushButton("Close", this);

    bottomLayout->addStretch();
    bottomLayout->addWidget(doNotShowAgainCheckbox, 0, Qt::AlignCenter);
    bottomLayout->addWidget(closeButton, 0, Qt::AlignCenter);
    bottomLayout->addStretch();

    layout->addLayout(bottomLayout);

    this->setStyleSheet("background-color: rgba(255, 255, 255, 240);");

    this->setLayout(layout);

    connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);

    connect(doNotShowAgainCheckbox, &QCheckBox::stateChanged, this, &HelpPage::onCheckboxChanged);
}

void HelpPage::populateHelpContent()
{

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
