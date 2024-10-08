#include "CardButton.h"

CardButton::CardButton(QWidget* parent)
    : QPushButton(parent), isActive(false), isHovered(false)
{
    // Initialize the animation object
    animation = new QPropertyAnimation(this, "pos");
    animation->setDuration(200);  // 200ms animation duration

    // Set a fixed size for the card
    setFixedSize(150, 200);  // Adjust the size as needed
}

void CardButton::setCardImage(const QPixmap& pixmap)
{
    cardImage = pixmap;
    update();  // Repaint the widget
}

void CardButton::setCardTitle(const QString& title)
{
    cardTitle = title;
    update();
}

void CardButton::setCardIcon(const QIcon& icon)
{
    cardIcon = icon;
    update();
}

void CardButton::setActive(bool active)
{
    isActive = active;
    updateAppearance();
}

void CardButton::returnToOriginalPosition()
{
    animation->stop();
    animation->setEndValue(originalPos);  // Return to the original position
    animation->start();
    isActive = false;
    updateAppearance();
}

void CardButton::setOriginalPos(const QPoint& pos)
{
    originalPos = pos;
}

void CardButton::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    // Draw the dark blue background
    painter.setBrush(QColor(0, 51, 102));  // Dark blue color
    painter.setPen(Qt::NoPen);  // No border for the background
    painter.drawRect(rect());

    // Draw the card image if set
    if (!cardImage.isNull()) {
        painter.drawPixmap(rect(), cardImage);
    }

    // Draw the card title
    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 16, QFont::Bold));
    painter.drawText(rect(), Qt::AlignCenter, cardTitle);

    // Draw the card icon if set
    if (!cardIcon.isNull()) {
        int iconSize = 32;
        cardIcon.paint(&painter, rect().width() - iconSize - 10, 10, iconSize, iconSize);
    }

    // Draw a white border around the card
    painter.setPen(QPen(Qt::white, 3));  // White border with 3px thickness
    painter.drawRect(rect().adjusted(1, 1, -2, -2));

    // Draw an additional yellow border if active
    if (isActive) {
        painter.setPen(QPen(Qt::yellow, 3));
        painter.drawRect(rect().adjusted(4, 4, -7, -7));
    }
}

void CardButton::enterEvent(QEnterEvent* event)
{
    Q_UNUSED(event);
    animation->stop();
    animation->setEndValue(originalPos + QPoint(0, -10)); // Move up by 10 pixels
    animation->start();
}

void CardButton::leaveEvent(QEvent* event)
{
    Q_UNUSED(event);
    animation->stop();
    animation->setEndValue(originalPos);  // Return to the original position
    animation->start();
}

void CardButton::mousePressEvent(QMouseEvent* event)
{
    emit cardClicked(this);  // Emit a signal when the card is clicked
    QPushButton::mousePressEvent(event);  // Call the base class implementation
}

void CardButton::mouseReleaseEvent(QMouseEvent* event)
{
    QPushButton::mouseReleaseEvent(event);  // Call the base class implementation
}

void CardButton::updateAppearance()
{
    update();  // Repaint the widget with the updated appearance
}
