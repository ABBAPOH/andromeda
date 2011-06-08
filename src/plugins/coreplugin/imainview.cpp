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
    Page *page = state()->object<Page>("page");
    if (page)
        page->setCurrentPath(path);
    else
        qWarning() << "State doesn't have Page class";
}
