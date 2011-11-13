#include "abstractview.h"

#include "abstractviewfactory.h"
#include "actionmanager.h"

using namespace GuiSystem;

/*!
  \class AbstractView

  \brief AbstractView is a base class different "panel" widgets, like docks.
*/

/*!
  \brief Creates an AbstractView with the given \a parent.
*/
AbstractView::AbstractView(QWidget *parent) :
    QWidget(parent),
    m_factory(0)
{
}

/*!
  \brief Destroys AbstractView and removes it from factory that created it.
*/
AbstractView::~AbstractView()
{
    if (m_factory)
        m_factory->m_views.removeAll(this);
}

/*!
  \property AbstractView::id

  \brief This property contains id of a factory that created this view, or
  empty byte array, if view was created using public constructor.
*/
QByteArray AbstractView::id() const
{
    if (m_factory)
        return m_factory->id();

    return QByteArray();
}

/*!
  \brief Adds \a action to this widget and registers it in ActionManager
  using an \a id.
*/
void AbstractView::addAction(QAction *action, const QByteArray &id)
{
    QWidget::addAction(action);
    registerAction(action, id);
}

/*!
  \property AbstractView::icon

  \brief Icon that can be shown on toolbars, menus and so on.
*/

/*!
  \brief Reimplement to return current view's icon.
*/
QIcon AbstractView::icon() const
{
    return QIcon();
}

/*!
  \fn void iconChanged(const QIcon &icon)

  \brief Emit this signal when view's icon is changed.
*/

/*!
  \property AbstractView::title

  \brief Title that can be show to user.
*/

/*!
  \brief Reimplement to return current view's title.

  Value returned by this function usually used as tab title.
*/
QString AbstractView::title() const
{
    return QString();
}

/*!
  \fn void titleChanged(const QString &title)

  \brief Emit this signal when view's title is changed.
*/

/*!
  \brief Reimplement to restore view's state from byte array \a state.

  Default implementation does nothing.
*/
void AbstractView::restoreState(const QByteArray &state)
{
    Q_UNUSED(state);
}

/*!
  \brief Reimplement to save view's state to byte array.

  Default implementation does nothing and returns empty byte array.
*/
QByteArray AbstractView::saveState() const
{
    return QByteArray();
}

/*!
  \brief Returns factory that created this view, or 0, if view was created
  using public constructor.
*/
AbstractViewFactory * AbstractView::factory() const
{
    return m_factory;
}

void AbstractView::setFactory(GuiSystem::AbstractViewFactory *factory)
{
    m_factory = factory;
}

/*!
  \brief Returns action manager instance.
*/
ActionManager * AbstractView::actionManager() const
{
    return ActionManager::instance();
}

/*!
  \brief Registers action in ActionManager.
*/
void AbstractView::registerAction(QAction *action, const QByteArray &id)
{
    actionManager()->registerAction(action, id);
}
