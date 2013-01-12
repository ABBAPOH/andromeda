#ifndef SHORTCUTEDIT_P_H
#define SHORTCUTEDIT_P_H

#include "shortcutedit.h"

class FilterLineEdit;
class QVBoxLayout;

class ShortcutEditPrivate
{
    Q_DECLARE_PUBLIC(ShortcutEdit)
public:
    ShortcutEditPrivate(ShortcutEdit *qq) : q_ptr(qq) {}

    FilterLineEdit *m_lineEdit;
    QVBoxLayout *m_layout;
    QKeySequence m_keySequence;
    int m_keyNum;
    int m_key[4];
    int m_prevKey;
    int releaseTimer;

    void init();
    int translateModifiers(Qt::KeyboardModifiers state, const QString &text);
    void resetState();
    void finishEditing();

private:
    ShortcutEdit *q_ptr;
};

#endif // SHORTCUTEDIT_P_H
