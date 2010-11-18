#include "editorview.h"

#include "editorhistory.h"

using namespace Core;

namespace Core {

class EditorViewPrivate
{
public:
    EditorHistory *history;

    void setUrl(const QString &path);
};

} // namespace Core

void EditorViewPrivate::setUrl(const QString &path)
{

}

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

void EditorView::open(const QString &path)
{
    Q_D(EditorView);
    d->setUrl(path);
}

void EditorView::close()
{
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
    QString path = d->history->itemAt(index).path();
    d->setUrl(path);
}
