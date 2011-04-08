#include "iview.h"

#include <QtCore/QDebug>
#include <QtGui/QWidget>

using namespace GuiSystem;

IView::IView(QObject *parent) :
    QObject(parent)
{
}

IView::~IView()
{
    if (!container())
        qWarning() << "GuiSystem::IView::~IView - container has already been deleted";
    delete m_container;
}

QString IView::factoryId() const
{
    return m_factoryId;
}

void IView::setFactoryId(const QString &id)
{
    m_factoryId = id;
}

QWidget * IView::container() const
{
    return m_container;
}

void IView::setContainer(QWidget *widget)
{
    m_container = widget;
}

