#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <QWidget>
#include <QHBoxLayout>
#include <QPushButton>
#include <QProgressBar>
#include <QIcon>
#include <QString>
#include "ResourceManager.h"

class Components {
public:
    // Constructor that accepts a ResourceManager pointer
    Components(ResourceManager* resourceManager);

    QWidget* createProgressBarWidget(int height = 1, QWidget* parent = nullptr) const;
    QPushButton* createButtonWithIcon(const QString& iconName, const QString& buttonStyle = defaultButtonStyle, int width = defaultButtonWidth, int height = defaultButtonHeight, QWidget* parent = nullptr) const;

    static const QString defaultButtonStyle;
    static const int defaultButtonWidth;
    static const int defaultButtonHeight;

private:
    ResourceManager* resourceManager; // Pointer to the ResourceManager instance
};

#endif // COMPONENTS_H
