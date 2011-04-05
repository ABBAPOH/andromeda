#include "iview.h"

using namespace GuiSystem;

IView::IView(QObject *parent) :
    QObject(parent)
{
}

IView::~IView()
{
}

QString IView::factoryId() const
{
    return m_factoryId;
}

void IView::setFactoryId(const QString &id)
{
    m_factoryId = id;
}

