#ifndef FANCYLINEEDIT_H
#define FANCYLINEEDIT_H

#include "widgets_global.h"

#include <QtGui/QAbstractButton>
#include <QtGui/QLineEdit>

class FancyLineEditPrivate;
class WIDGETS_EXPORT FancyLineEdit : public QLineEdit
{
    Q_OBJECT
    Q_DISABLE_COPY(FancyLineEdit)
public:
    enum Side { Left = 0, Right = 1 };
    Q_ENUMS(Side)

    explicit FancyLineEdit(QWidget *parent = 0);
    ~FancyLineEdit();

    QMenu *buttonMenu(Side side) const;
    void setButtonMenu(Side side, QMenu *menu);

    QPixmap buttonPixmap(Side side) const;
    void setButtonPixmap(Side side, const QPixmap &pixmap);

    bool isButtonVisible(Side side) const;
    void setButtonVisible(Side side, bool visible);

    void setButtonToolTip(Side side, const QString &);
    void setButtonFocusPolicy(Side side, Qt::FocusPolicy policy);

    bool hasMenuTabFocusTrigger(Side side) const;
    void setMenuTabFocusTrigger(Side side, bool v);

    bool hasAutoHideButton(Side side) const;
    void setAutoHideButton(Side side, bool h);

signals:
    void buttonClicked(FancyLineEdit::Side side);
    void leftButtonClicked();
    void rightButtonClicked();

protected:
    virtual void resizeEvent(QResizeEvent *e);

private slots:
    void checkButtons(const QString &);
    void iconClicked();

private:
    void updateMargins();
    void updateButtonPositions();
    friend class FancyLineEditPrivate;

private:
    FancyLineEditPrivate *d;
};

#endif // FANCYLINEEDIT_H
