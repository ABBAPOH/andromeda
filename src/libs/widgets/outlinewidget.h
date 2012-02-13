#ifndef OUTLINEWIDGET_H
#define OUTLINEWIDGET_H

#include "widgets_global.h"

#include <QtGui/QFrame>

class OutlineWidgetPrivate;
class WIDGETS_EXPORT OutlineWidget : public QFrame
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(OutlineWidget)
    Q_DISABLE_COPY(OutlineWidget)

    Q_PROPERTY(bool animated READ isAnimated WRITE setAnimated)

public:
    explicit OutlineWidget(QWidget *parent = 0);
    ~OutlineWidget();

    int addWidget(QWidget *widget, const QString &text);
    int addWidget(QWidget *widget, const QIcon &icon, const QString &text);

    bool isAnimated() const;
    void setAnimated(bool animated);

    void clear();
    int	count() const;

    QIcon icon(int index) const;
    void setIcon(int index, const QIcon & icon);

    int insertWidget(int index, QWidget * widget, const QString & label);
    int insertWidget(int index, QWidget * widget, const QIcon & icon, const QString & label);

    void remove(int index);

    QString text(int index) const;
    void setText(int index, const QString &text);

    int indexOf(QWidget * w) const;
    QWidget *widget(int index) const;

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

public slots:
    void collapse(int index);
    void collapseAll();
    void expand(int index);
    void expandAll();

protected:
    bool eventFilter(QObject *, QEvent *);
    void resizeEvent(QResizeEvent *);

protected:
    OutlineWidgetPrivate *d_ptr;
};

#endif // OUTLINEWIDGET_H
