#include "editorview.h"

#include "ieditor.h"
#include "editormanager.h"
#include "editorhistory.h"
#include "ieditor.h"
#include "../core.h"
#include "../ifile.h"

#include <QResizeEvent>
#include <QDebug>

using namespace CorePlugin;

namespace CorePlugin {

class EditorViewPrivate
{
public:
    EditorHistory *history;
    IEditor *editor;
    QWidget *widget;
};

} // namespace CorePlugin

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

EditorView::~EditorView()
{
    qDebug("~EditorView");
    Q_D(EditorView);
    delete d->editor;
}

EditorHistory *EditorView::history()
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
        if (d->widget)
            d->widget->deleteLater(); // save from stupid plugin coder
        disconnect(d->editor, SIGNAL(destroyed()), this, SLOT(clearEditor()));
        delete d->editor;
        d->editor = newEditor;
        connect(d->editor, SIGNAL(destroyed()), SLOT(clearEditor()));
        d->widget = newEditor->widget();
        d->widget->setParent(this);
        d->widget->show();
        IFile *file = d->editor->file();
        if (file) {
            if (addToHistory) {
                d->history->appendItem(HistoryItem(file->icon(),
                                                   QDateTime::currentDateTime(),
                                                   file->name(),
                                                   file->path()));
            }
        }
        return true;
    }
    return false;
}

void EditorView::close()
{
    Q_D(EditorView);
    d->editor->close();
    if (d->editor->file() == 0) {
        // we have no more opened files
        delete d->editor;
        d->widget->deleteLater(); // to be sure if plugin coder not idiot
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

void EditorView::clearEditor()
{
    qDebug("EditorView::clearEditor");
    Q_D(EditorView);
    d->editor = 0;
    if (d->widget)
        d->widget->deleteLater();
    d->widget = 0;
}

void EditorView::resizeEvent(QResizeEvent *event)
{
    Q_D(EditorView);
    if (d->widget)
        d->widget->resize(event->size());
}
