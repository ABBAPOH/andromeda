#ifndef STACKEDCONTAINER_P_H
#define STACKEDCONTAINER_P_H

#include "stackedcontainer.h"

#include "abstracteditorfactory.h"
#include "editormanager.h"
#include "proxyfile.h"
#include "stackedhistory.h"

class QStackedLayout;

namespace GuiSystem {

class StackedContainerPrivate
{
    Q_DECLARE_PUBLIC(StackedContainer)
public:
    StackedContainerPrivate(StackedContainer *qq) : q_ptr(qq) {}

    QStackedLayout *layout;
    QUrl currentUrl;
    AbstractEditor *editor;
    QHash<QString, AbstractEditor *> editorHash;
    ProxyFile *file;
    StackedHistory *stackedHistory;
    bool ignoreSignals;

protected:
    StackedContainer *q_ptr;
};

} // namespace GuiSystem

#endif // STACKEDCONTAINER_P_H
