#ifndef FANCYLINEEDIT_P_H
#define FANCYLINEEDIT_P_H

#include "fancylineedit.h"

class WIDGETS_EXPORT IconButton: public QAbstractButton
{
    Q_OBJECT
    Q_DISABLE_COPY(IconButton)

    Q_PROPERTY(bool autoHide READ hasAutoHide WRITE setAutoHide)
    Q_PROPERTY(float iconOpacity READ iconOpacity WRITE setIconOpacity)
    Q_PROPERTY(QPixmap pixmap READ pixmap WRITE setPixmap)

public:
    explicit IconButton(QWidget *parent = 0);

    void animateShow(bool visible);

    bool hasAutoHide() const { return m_autoHide; }
    void setAutoHide(bool hide) { m_autoHide = hide; }

    float iconOpacity() { return m_iconOpacity; }
    void setIconOpacity(float value) { m_iconOpacity = value; update(); }

    QPixmap pixmap() const { return m_pixmap; }
    void setPixmap(const QPixmap &pixmap) { m_pixmap = pixmap; update(); }

protected:
    void paintEvent(QPaintEvent *event);

private:
    float m_iconOpacity;
    bool m_autoHide;
    QPixmap m_pixmap;
};

class FancyLineEditPrivate : public QObject
{
public:
    explicit FancyLineEditPrivate(FancyLineEdit *parent);

    virtual bool eventFilter(QObject *obj, QEvent *event);

public:
    FancyLineEdit  *m_lineEdit;
    QPixmap m_pixmap[2];
    QMenu *m_menu[2];
    bool m_menuTabFocusTrigger[2];
    IconButton *m_iconbutton[2];
    bool m_iconEnabled[2];
    QString m_oldText;
};

#endif // FANCYLINEEDIT_P_H
