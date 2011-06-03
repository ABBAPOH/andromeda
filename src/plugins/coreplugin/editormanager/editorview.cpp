#include "editorview.h"

#include "ieditor.h"
#include "editormanager.h"
#include "../history.h"
#include "ieditor.h"
#include "../core.h"
#include "../ifile.h"

#include <QPointer>
#include <QResizeEvent>
#include <QDebug>

using namespace CorePlugin;

namespace CorePlugin {

class EditorViewPrivate
{
public:
    History *history;
    QPointer<IEditor> editor;
    QWidget *widget;
};

} // namespace CorePlugin

EditorView::EditorView(QWidget *parent) :
    QWidget(parent),
    d_ptr(new EditorViewPrivate)
{
    Q_D(EditorView);
    d->history = new History(this);
    d->editor = 0;
    d->widget = 0;

    connect(d->history, SIGNAL(currentItemIndexChanged(int)), SLOT(onCurrentItemIndexChanged(int)));
}

EditorView::~EditorView()
{
    qDebug("~EditorView");
    Q_D(EditorView);
    delete d->editor;
}

History *EditorView::history()
{
    return d_func()->history;
}

bool EditorView::open(const QString &path, bool addToHistory)
{
    Q_D(EditorView);
//    d->setPath(path);
    EditorManager *manager = Core::instance()->editorManager();
    IEditor *newEditor = manager->openEditor(path);
    qDebug("open");
    if (newEditor) {
        delete d->editor;
        d->editor = newEditor;
        d->widget = newEditor->widget();
        d->widget->setParent(this);
        d->widget->show();
        IFile *file = d->editor->file();
        if (file) {
            if (addToHistory) {
                d->history->appendItem(HistoryItem(file->path(),
                                                   file->name(),
                                                   file->icon(),
                                                   QDateTime::currentDateTime()));
            }
        }
        return true;
    }
    return false;
}

void EditorView::close()
{
    Q_D(EditorView);
    if (d->editor)
        d->editor->close();
    if (d->editor->file() == 0) {
        // we have no more opened files
        delete d->editor;
        d->editor = 0;
        d->widget = 0;
    }
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
    open(path, false);
}

void EditorView::resizeEvent(QResizeEvent *event)
{
    Q_D(EditorView);
    if (d->widget)
        d->widget->resize(event->size());
}
