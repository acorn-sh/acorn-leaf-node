#include "Components.h"

// Define static constants for button style and size
const QString Components::defaultButtonStyle = "QPushButton { background-color: #938ea4; }"
                                               "QPushButton:hover { background-color: #fbdea3; }"
                                               "QPushButton:pressed { background-color: #6d6781; }";
const int Components::defaultButtonWidth = 24;
const int Components::defaultButtonHeight = 24;

// Constructor implementation
Components::Components(ResourceManager* resourceManager)
    : resourceManager(resourceManager) {}

// Implementation of createProgressBarWidget function
QWidget* Components::createProgressBarWidget(int height, QWidget* parent) const {
    QWidget* progressWidget = new QWidget(parent);
    QHBoxLayout* layout = new QHBoxLayout(progressWidget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setAlignment(Qt::AlignCenter);

    QProgressBar* progressBar = new QProgressBar();
    progressBar->setRange(0, 0);
    progressBar->setFixedHeight(height);
    progressBar->setTextVisible(false);

    layout->addWidget(progressBar);
    progressWidget->setLayout(layout);

    return progressWidget;
}

// Implementation of createButtonWithIcon function
QPushButton* Components::createButtonWithIcon(const QString& iconName, const QString& buttonStyle, int width, int height, QWidget* parent) const {
    QPushButton* button = new QPushButton(parent);
    button->setIcon(resourceManager->getIcon(iconName));
    button->setFixedSize(width, height);

    if (!buttonStyle.isEmpty()) {
        button->setStyleSheet(buttonStyle);
    }

    return button;
}
