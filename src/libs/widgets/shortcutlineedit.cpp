#include "shortcutlineedit.h"

#include <QtGui/QKeyEvent>

ShortcutLineEdit::ShortcutLineEdit(QWidget *parent) :
    FilterLineEdit(parent),
    m_keyNum(0),
    m_prevKey(-1)
{
    m_key[0] = m_key[1] = m_key[2] = m_key[3] = 0;
    setAttribute(Qt::WA_InputMethodEnabled, false);

    connect(this, SIGNAL(buttonClicked(FancyLineEdit::Side)), SLOT(clearKeySequence()));
}

QKeySequence ShortcutLineEdit::keySequence() const
{
    return QKeySequence(text());
}

void ShortcutLineEdit::setKeySequence(const QKeySequence &key)
{
    m_keyNum = m_key[0] = m_key[1] = m_key[2] = m_key[3] = 0;
    m_keyNum = key.count();
    for (int i = 0; i < m_keyNum; ++i) {
        m_key[i] = key[i];
    }

    setText(key);
}

void ShortcutLineEdit::clearKeySequence()
{
    m_keyNum = m_key[0] = m_key[1] = m_key[2] = m_key[3] = 0;
    m_keyNum = 0;
    FilterLineEdit::clear();
}

bool ShortcutLineEdit::event(QEvent *e)
{
    switch (e->type()) {
    case QEvent::KeyPress : {
        keyPressEvent(static_cast<QKeyEvent*>(e));
        return true;
    }
    case QEvent::KeyRelease : {
        keyReleaseEvent(static_cast<QKeyEvent*>(e));
        return true;
    }
    case QEvent::Shortcut : {
        return true;
    }
    case QEvent::ShortcutOverride : {
        // for shortcut overrides, we need to accept as well
        e->accept();
        return true;
    }
    default : break;
    }

    return QLineEdit::event(e);
}

void ShortcutLineEdit::keyPressEvent(QKeyEvent *e)
{
    int nextKey = e->key();

    if (m_prevKey == -1) {
        clearKeySequence();
        m_prevKey = nextKey;
    }

    if ( nextKey == Qt::Key_Control ||
         nextKey == Qt::Key_Shift ||
         nextKey == Qt::Key_Meta ||
         nextKey == Qt::Key_Alt )
         return;

    if (!selectedText().isEmpty() && selectedText() == text()) {
        clearKeySequence();
        if (nextKey == Qt::Key_Backspace)
            return;
    }

    if ( m_keyNum > 3)
        return;

    nextKey |= translateModifiers(e->modifiers(), e->text());
    switch (m_keyNum) {
        case 0:
            m_key[0] = nextKey;
            break;
        case 1:
            m_key[1] = nextKey;
            break;
        case 2:
            m_key[2] = nextKey;
            break;
        case 3:
            m_key[3] = nextKey;
            break;
        default:
            break;
    }
    m_keyNum++;
    QKeySequence ks(m_key[0], m_key[1], m_key[2], m_key[3]);
    setText(ks);
    e->accept();
}

void ShortcutLineEdit::keyReleaseEvent(QKeyEvent *e)
{
    if (m_prevKey == e->key()) {
        m_prevKey = -1;
        emit shortcutFinished();
    }
}

int ShortcutLineEdit::translateModifiers(Qt::KeyboardModifiers state, const QString &text)
{
    int result = 0;
    // The shift modifier only counts when it is not used to type a symbol
    // that is only reachable using the shift key anyway
    if ((state & Qt::ShiftModifier) && (text.size() == 0 ||
                                        !text.at(0).isPrint() ||
                                        text.at(0).isLetterOrNumber() ||
                                        text.at(0).isSpace()))
        result |= Qt::SHIFT;
    if (state & Qt::ControlModifier)
        result |= Qt::CTRL;
    if (state & Qt::MetaModifier)
        result |= Qt::META;
    if (state & Qt::AltModifier)
        result |= Qt::ALT;
    return result;
}
