#include "shortcutedit.h"
#include "shortcutedit_p.h"

#include <QtGui/QKeyEvent>
#include <QtGui/QVBoxLayout>

#include "filterlineedit.h"

void ShortcutEditPrivate::init()
{
    Q_Q(ShortcutEdit);

    m_lineEdit = new FilterLineEdit(q);
    m_keyNum = 0;
    m_prevKey = -1;
    releaseTimer = 0;

    m_layout = new QVBoxLayout(q);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(0);
    m_layout->addWidget(m_lineEdit);

    m_key[0] = m_key[1] = m_key[2] = m_key[3] = 0;

    m_lineEdit->setFocusProxy(q);
    m_lineEdit->installEventFilter(q);
    m_lineEdit->setPlaceholderText(ShortcutEdit::tr("Press shortcut"));
    q->connect(m_lineEdit, SIGNAL(buttonClicked(FancyLineEdit::Side)), q, SLOT(clearKeySequence()));

    q->setAttribute(Qt::WA_InputMethodEnabled, false);
    q->setFocusPolicy(Qt::StrongFocus);
}

int ShortcutEditPrivate::translateModifiers(Qt::KeyboardModifiers state, const QString &text)
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

void ShortcutEditPrivate::resetState()
{
    Q_Q(ShortcutEdit);

    if (releaseTimer) {
        q->killTimer(releaseTimer);
        releaseTimer = 0;
        m_prevKey = -1;
        m_lineEdit->setText(m_keySequence.toString(QKeySequence::NativeText));
        m_lineEdit->setPlaceholderText(ShortcutEdit::tr("Press shortcut"));
    }
}

void ShortcutEditPrivate::finishEditing()
{
    Q_Q(ShortcutEdit);

    resetState();
    emit q->keySequenceChanged(m_keySequence);
    emit q->keySequenceFinished();
}

ShortcutEdit::ShortcutEdit(QWidget *parent) :
    QWidget(parent),
    d_ptr(new ShortcutEditPrivate(this))
{
    Q_D(ShortcutEdit);
    d->init();
}

ShortcutEdit::~ShortcutEdit()
{
    delete d_ptr;
}

QKeySequence ShortcutEdit::keySequence() const
{
    Q_D(const ShortcutEdit);

    return d->m_keySequence;
}

void ShortcutEdit::setKeySequence(const QKeySequence &key)
{
    Q_D(ShortcutEdit);

    d->resetState();

    if (d->m_keySequence == key)
        return;

    d->m_keyNum = d->m_key[0] = d->m_key[1] = d->m_key[2] = d->m_key[3] = 0;
    d->m_keyNum = key.count();
    for (int i = 0; i < d->m_keyNum; ++i)
        d->m_key[i] = key[i];

    d->m_lineEdit->setText(key);

    emit keySequenceChanged(key);
}

void ShortcutEdit::clearKeySequence()
{
    Q_D(ShortcutEdit);

    d->resetState();

    d->m_lineEdit->clear();
    d->m_keySequence = QKeySequence();
    d->m_keyNum = d->m_key[0] = d->m_key[1] = d->m_key[2] = d->m_key[3] = 0;
    d->m_prevKey = -1;
    emit keySequenceChanged(d->m_keySequence);
}

bool ShortcutEdit::event(QEvent *e)
{
    switch (e->type()) {
    case QEvent::Shortcut :
        return true;
    case QEvent::ShortcutOverride :
        // for shortcut overrides, we need to accept as well
        e->accept();
        return true;
    default :
        break;
    }

    return QWidget::event(e);
}

void ShortcutEdit::keyPressEvent(QKeyEvent *e)
{
    Q_D(ShortcutEdit);

    int nextKey = e->key();

    if (d->m_prevKey == -1) {
        clearKeySequence();
        d->m_prevKey = nextKey;
    }

    d->m_lineEdit->setPlaceholderText(QString());
    if (nextKey == Qt::Key_Control
            || nextKey == Qt::Key_Shift
            || nextKey == Qt::Key_Meta
            || nextKey == Qt::Key_Alt)
        return;

    QString selectedText = d->m_lineEdit->selectedText();
    if (!selectedText.isEmpty() && selectedText == d->m_lineEdit->text()) {
        clearKeySequence();
        if (nextKey == Qt::Key_Backspace)
            return;
    }

    if (d->m_keyNum > 3)
        return;

    nextKey |= d->translateModifiers(e->modifiers(), e->text());

    if (d->m_keyNum < 4)
        d->m_key[d->m_keyNum] = nextKey;
    d->m_keyNum++;

    QKeySequence key(d->m_key[0], d->m_key[1], d->m_key[2], d->m_key[3]);
    d->m_keySequence = key;
    QString text = key.toString(QKeySequence::NativeText);
    if (d->m_keyNum < 4)
        text.append(QLatin1String(", ..."));
    d->m_lineEdit->setText(text);
    e->accept();
}

void ShortcutEdit::keyReleaseEvent(QKeyEvent *e)
{
    Q_D(ShortcutEdit);

    if (d->m_prevKey == e->key()) {
        if (d->m_keyNum < 4)
            d->releaseTimer = startTimer(1000);
        else
            d->finishEditing();
    }
}

void ShortcutEdit::timerEvent(QTimerEvent *e)
{
    Q_D(ShortcutEdit);

    QTimerEvent *te = static_cast<QTimerEvent *>(e);
    if (te->timerId() == d->releaseTimer) {
        d->finishEditing();
        return;
    }
}
