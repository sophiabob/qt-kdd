#ifndef DOUBLECLICKBUTTON_H
#define DOUBLECLICKBUTTON_H

#include <QPushButton>

class DoubleClickButton : public QPushButton {
    Q_OBJECT
public:
    explicit DoubleClickButton(QWidget *parent = nullptr);  // explicit допустим (это конструктор)
    explicit DoubleClickButton(const QString &text, QWidget *parent = nullptr);  // тоже конструктор
signals:
    void doubleClicked();
protected:
    void mouseDoubleClickEvent(QMouseEvent *event) override;
};

#endif // DOUBLECLICKBUTTON_H
