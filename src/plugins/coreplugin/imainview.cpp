#include "imainview.h"

#include "page.h"

#include <state.h>
#include <QDebug>

using namespace CorePlugin;

IMainView::IMainView(QObject *parent) :
    GuiSystem::IView(parent)
{
    connect(this, SIGNAL(pathChanged(QString)), SLOT(onPathChanged(QString)));
}

void IMainView::onPathChanged(const QString &path)
{
    QObject *o = state()->object("page");
    Page *page = qobject_cast<Page*>(o);
    if (page)
        page->setCurrentPath(path);
    else
        qWarning() << "State doesn't have Page class";
}
