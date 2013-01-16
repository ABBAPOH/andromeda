#include "toolwidgetfactory.h"

using namespace GuiSystem;

/*!
    \class ToolWidgetFactory

    Base class for instantiating ToolWidget objects.
*/

/*!
    Created ToolWidgetFactory with the given \a id and \a parent.

    \note \a id can't be empty.
*/
ToolWidgetFactory::ToolWidgetFactory(const QByteArray &id, QObject *parent) :
    QObject(parent),
    m_id(id)
{
    Q_ASSERT_X(!id.isEmpty(),
               "ToolWidgetFactory::ToolWidgetFactory",
               "Id can't be empty");
}

/*!
    Returns factory's id.
*/
QByteArray ToolWidgetFactory::id() const
{
    return m_id;
}

/*!
    Reimplement to return the default area this tool should be added to.
*/
ToolWidgetFactory::Area ToolWidgetFactory::defaultArea() const
{
    return LeftArea;
}

/*!
    \fn ToolWidget * ToolWidgetFactory::createToolWidget(QWidget *parent = 0) const;

    Reimplement this function to create new ToolWidget.
*/
