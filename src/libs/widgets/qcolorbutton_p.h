#ifndef COLORBUTTON_P_H
#define COLORBUTTON_P_H

#include "qcolorbutton.h"

class QColorButtonPrivate
{
    QColorButton *q_ptr;
    Q_DECLARE_PUBLIC(QColorButton)

public:
    QColorButtonPrivate(QColorButton *qq);

    void slotEditColor();
    QColor shownColor() const;
    QPixmap generatePixmap() const;

public:
    QColor m_color;
#ifndef QT_NO_DRAGANDDROP
    QColor m_dragColor;
    QPoint m_dragStart;
    bool m_dragging;
    bool dragDropEnabled;
#endif
    bool m_backgroundCheckered;
    bool m_alphaAllowed;
};

#endif // COLORBUTTON_P_H
