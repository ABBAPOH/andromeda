#ifndef EDITORVIEW_P_H
#define EDITORVIEW_P_H

#include <QtCore/QPointer>

#include "editorview.h"

#include "abstracteditorfactory.h"
#include "editormanager.h"
#include "editorviewhistory.h"

class QStackedLayout;

namespace GuiSystem {

class FindToolBar;

class EditorViewPrivate
{
    Q_DECLARE_PUBLIC(EditorView)
public:
    EditorViewPrivate(EditorView *qq) : q_ptr(qq) {}
    void init();

    void openEditor(const QUrl &url, AbstractEditorFactory *factory);

    FindToolBar *findToolBar;
    QLayout *mainLayout;
    QStackedLayout *layout;
    QUrl currentUrl;
    QPointer<AbstractEditor> editor;
    AbstractDocument *document;
    EditorViewHistory *stackedHistory;
    QAction *findAction;
    bool ignoreSignals;

protected:
    EditorView *q_ptr;
};

} // namespace GuiSystem

#endif // EDITORVIEW_P_H
