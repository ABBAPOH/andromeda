#include "editorview.h"

#include "editormanager.h"
#include "editorhistory.h"
#include "ieditor.h"
#include "icore.h"

#include <QResizeEvent>

using namespace Core;

namespace Core {

class EditorViewPrivate
{
public:
    EditorHistory *history;
    IEditor *editor;
    QWidget *widget;

    void setPath(const QString &path);
};

} // namespace Core

void EditorViewPrivate::setPath(const QString &path)
{
}

EditorView::EditorView(QWidget *parent) :
    QWidget(parent),
    d_ptr(new EditorViewPrivate)
{
    Q_D(EditorView);
    d->history = new EditorHistory(this);
    d->editor = 0;
    d->widget = 0;

    connect(d->history, SIGNAL(currentItemIndexChanged(int)), SLOT(onCurrentItemIndexChanged(int)));
}

EditorHistory *EditorView::history()
{
    return d_func()->history;
}

void EditorView::open(const QString &path)
{
    Q_D(EditorView);
//    d->setPath(path);
    EditorManager *manager = ICore::instance()->editorManager();
    IEditor *newEditor = manager->openEditor(path);
    qDebug("setPath");
    if (newEditor) {
        delete d->editor;
        delete d->widget;
        d->editor = newEditor;
        d->widget = newEditor->widget();
        d->widget->setParent(this);
        d->widget->show();
    }
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
    d->setPath(path);
}

void EditorView::resizeEvent(QResizeEvent *event)
{
    Q_D(EditorView);
    if (d->widget)
        d->widget->resize(event->size());
}
