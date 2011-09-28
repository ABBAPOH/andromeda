#ifndef PROXYACTION_H
#define PROXYACTION_H

#include "guisystem_global.h"

#include <QtCore/QPointer>
#include <QtGui/QAction>

namespace GuiSystem {

class GUISYSTEM_EXPORT ProxyAction : public QAction
{
    Q_OBJECT
    Q_DISABLE_COPY(ProxyAction)

public:
    enum Attribute {
        Hide = 0x01,
        UpdateText = 0x02,
        UpdateIcon = 0x04
    };
    Q_DECLARE_FLAGS(Attributes, Attribute)

    explicit ProxyAction(QObject *parent = 0);

    void initialize(QAction *action);

    void setAction(QAction *action);
    QAction *action() const;

    Attributes attributes() const;
    void setAttributes(Attributes attributes);

    bool hasAttribute(Attribute attribute) const;
    void setAttribute(Attribute attribute);
    void removeAttribute(Attribute attribute);

    bool shortcutVisibleInToolTip() const;
    void setShortcutVisibleInToolTip(bool visible);

    static QString stringWithAppendedShortcut(const QString &str, const QKeySequence &shortcut);

private slots:
    void actionChanged();
    void updateState();
    void updateToolTipWithKeySequence();

private:
    void disconnectAction();
    void connectAction();
    void update(QAction *action, bool initialize);

    QPointer<QAction> m_action;
    Attributes m_attributes;
    bool m_showShortcut;
    QString m_toolTip;
    bool m_block;
};

} // namespace GuiSystem

Q_DECLARE_OPERATORS_FOR_FLAGS(GuiSystem::ProxyAction::Attributes)

#endif // PROXYACTION_H
