#include "ieditor.h"

#include <QtCore/QSettings>

using namespace CorePlugin;

IEditor::IEditor(QObject *parent) :
    GuiSystem::IView(parent)
{
}

QString IEditor::currentPath() const
{
    return currentItem().path();
}

void IEditor::restoreSession(const QSettings &s)
{
    open(s.value(QLatin1String("currentPath")).toString());
}

void IEditor::saveSession(QSettings &s)
{
    s.setValue(QLatin1String("currentPath"), currentPath());
}
