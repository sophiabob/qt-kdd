#include "doubleclickbutton.h"
#include <QMouseEvent>

DoubleClickButton::DoubleClickButton(QWidget *parent)
    : QPushButton(parent)
{}

DoubleClickButton::DoubleClickButton(const QString &text, QWidget *parent)
    : QPushButton(text, parent)
{}

void DoubleClickButton::mouseDoubleClickEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        emit doubleClicked();
    }
    QPushButton::mouseDoubleClickEvent(event);
}
