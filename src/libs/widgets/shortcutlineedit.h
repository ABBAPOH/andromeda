#ifndef SHORTCUTLINEEDIT_H
#define SHORTCUTLINEEDIT_H

#include "filterlineedit.h"

class WIDGETS_EXPORT ShortcutLineEdit : public FilterLineEdit
{
    Q_OBJECT
public:
    explicit ShortcutLineEdit(QWidget *parent = 0);

    QKeySequence keySequence() const;
    void setKeySequence(const QKeySequence &key);

public slots:
    void clearKeySequence();

signals:
    void shortcutFinished();

protected:
    bool event(QEvent *);
    void keyPressEvent(QKeyEvent *);
    void keyReleaseEvent(QKeyEvent *);

private:
    int translateModifiers(Qt::KeyboardModifiers state, const QString &text);

private:
    int m_keyNum;
    int m_key[4];
    int m_prevKey;
};

#endif // SHORTCUTLINEEDIT_H
