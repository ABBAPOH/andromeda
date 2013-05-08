#include "proxyaction.h"

using namespace GuiSystem;

ProxyAction::ProxyAction(QObject *parent) :
    QAction(parent),
    m_action(0),
    m_attributes(0),
    m_showShortcut(false),
    m_block(false)
{
    connect(this, SIGNAL(changed()), this, SLOT(updateToolTipWithKeySequence()));
    updateState();
}

void ProxyAction::setAction(QAction *action)
{
    if (m_action == action)
        return;
    disconnectAction();
    m_action = action;
    connectAction();
    updateState();
}

QAction *ProxyAction::action() const
{
    return m_action;
}

bool ProxyAction::hasAttribute(ProxyAction::Attribute attribute) const
{
    return (m_attributes & attribute);
}

ProxyAction::Attributes ProxyAction::attributes() const
{
    return m_attributes;
}

void ProxyAction::setAttribute(ProxyAction::Attribute attribute)
{
    m_attributes |= attribute;
    updateState();
}

void ProxyAction::removeAttribute(ProxyAction::Attribute attribute)
{
    m_attributes &= ~attribute;
    updateState();
}

void ProxyAction::setAttributes(Attributes attributes)
{
    m_attributes = attributes;
    updateState();
}

void ProxyAction::initialize(QAction *action)
{
    update(action, true);
}

bool ProxyAction::shortcutVisibleInToolTip() const
{
    return m_showShortcut;
}

void ProxyAction::setShortcutVisibleInToolTip(bool visible)
{
    m_showShortcut = visible;
    updateToolTipWithKeySequence();
}

QString ProxyAction::stringWithAppendedShortcut(const QString &str, const QKeySequence &shortcut)
{
    return QString("%1 <span style=\"color: gray; font-size: small\">%2</span>").arg(str).arg(
                shortcut.toString(QKeySequence::NativeText));
}

void ProxyAction::actionChanged()
{
    update(m_action, false);
}

void ProxyAction::updateState()
{
    if (m_action) {
        update(m_action, false);
    } else {
        // no active/delegate action, "visible" action is not enabled/visible
        if (hasAttribute(Hide))
            setVisible(false);
        setEnabled(false);
    }
}

void ProxyAction::updateToolTipWithKeySequence()
{
    if (m_block)
        return;
    m_block = true;
    if (!m_showShortcut || shortcut().isEmpty())
        setToolTip(m_toolTip);
    else
        setToolTip(stringWithAppendedShortcut(m_toolTip, shortcut()));
    m_block = false;
}

void ProxyAction::disconnectAction()
{
    if (m_action) {
        disconnect(m_action, SIGNAL(changed()), this, SLOT(actionChanged()));
        disconnect(this, SIGNAL(triggered(bool)), m_action, SIGNAL(triggered(bool)));
        disconnect(this, SIGNAL(toggled(bool)), m_action, SLOT(setChecked(bool)));
    }
}

void ProxyAction::connectAction()
{
    if (m_action) {
        connect(m_action, SIGNAL(changed()), this, SLOT(actionChanged()));
        connect(this, SIGNAL(triggered(bool)), m_action, SIGNAL(triggered(bool)));
        connect(this, SIGNAL(toggled(bool)), m_action, SLOT(setChecked(bool)));
    }
}

void ProxyAction::update(QAction *action, bool initialize)
{
    if (!action)
        return;
    disconnectAction();
    disconnect(this, SIGNAL(changed()), this, SLOT(updateToolTipWithKeySequence()));
    if (initialize)
        setSeparator(action->isSeparator());
    if (hasAttribute(UpdateIcon) || initialize) {
        setIcon(action->icon());
        setIconText(action->iconText());
        setIconVisibleInMenu(action->isIconVisibleInMenu());
    }
    if (hasAttribute(UpdateText) || initialize) {
        setText(action->text());
        m_toolTip = action->toolTip();
        updateToolTipWithKeySequence();
        setStatusTip(action->statusTip());
        setWhatsThis(action->whatsThis());
    }

    setCheckable(action->isCheckable());

    if (!initialize) {
        setChecked(action->isChecked());
        setEnabled(action->isEnabled());
        setVisible(action->isVisible());
    }
    connectAction();
    connect(this, SIGNAL(changed()), this, SLOT(updateToolTipWithKeySequence()));
}

