#ifndef SHORTCUTEDIT_H
#define SHORTCUTEDIT_H

#include "widgets_global.h"

#include <QtGui/QWidget>

class ShortcutEditPrivate;
class WIDGETS_EXPORT ShortcutEdit : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(ShortcutEdit)
    Q_DECLARE_PRIVATE(ShortcutEdit)

    Q_PROPERTY(QKeySequence keySequence READ keySequence WRITE setKeySequence USER true)
public:
    explicit ShortcutEdit(QWidget *parent = 0);
    ~ShortcutEdit();

    QKeySequence keySequence() const;
    void setKeySequence(const QKeySequence &key);

public slots:
    void clearKeySequence();

signals:
    void shortcutFinished();
    void keySequenceChanged(const QKeySequence &key);

protected:
    bool event(QEvent *);
    void keyPressEvent(QKeyEvent *);
    void keyReleaseEvent(QKeyEvent *);

protected:
    ShortcutEditPrivate *d_ptr;
};

#endif // SHORTCUTEDIT_H
