#include "editorview.h"

#include "editorhistory.h"

using namespace Core;

namespace Core {

class EditorViewPrivate
{
public:
    EditorHistory *history;

    void setUrl(const QUrl &url);
};

} // namespace Core

EditorView::EditorView(QWidget *parent) :
    QWidget(parent),
    d_ptr(new EditorViewPrivate)
{
    Q_D(EditorView);
    d->history = new EditorHistory(this);
    connect(d->history, SIGNAL(currentItemIndexChanged(int)), SLOT(onCurrentItemIndexChanged(int)));
}

EditorHistory *EditorView::history()
{
    return d_func()->history;
}

void EditorView::open(const QUrl &url)
{
    Q_D(EditorView);
    d->setUrl(url);
}

void EditorView::back()
{
    d_func()->history->back();
}

void EditorView::forward()
{
    d_func()->history->forward();
}

void EditorView::onCurrentItemIndexChanged(int index)
{
    Q_D(EditorView);
    QUrl url = d->history->itemAt(index).url();
    d->setUrl(url);
}
