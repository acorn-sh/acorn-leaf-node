#ifndef CARDBUTTON_H
#define CARDBUTTON_H

#include <QWidget>
#include <QIcon>
#include <QPixmap>
#include <QPainter>
#include <QMouseEvent>
#include <QEnterEvent>
#include <QPushButton>
#include <QPoint>
#include <QPropertyAnimation>

class CardButton : public QPushButton
{
    Q_OBJECT

public:
    explicit CardButton(QWidget* parent = nullptr);
    void setCardImage(const QPixmap& pixmap);
    void setCardTitle(const QString& title);
    void setCardIcon(const QIcon& icon);
    void setActive(bool active);

    void returnToOriginalPosition();
    void setOriginalPos(const QPoint& pos);

signals:
    void cardClicked(CardButton* card);

protected:
    void paintEvent(QPaintEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    QPixmap cardImage;
    QString cardTitle;
    QIcon cardIcon;
    QPoint originalPos;  // Store the original position of the button

    bool isActive;
    bool isHovered;

    QPropertyAnimation* animation;  // Animation for smooth movements

    void updateAppearance();
};

#endif // CARDBUTTON_H
