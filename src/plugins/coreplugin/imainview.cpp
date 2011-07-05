#include "imainview.h"

#include <QDebug>

using namespace CorePlugin;

IMainView::IMainView(QObject *parent) :
    GuiSystem::IView(parent)
{
}

void IMainView::onPathChanged(const QString &path)
{
}
