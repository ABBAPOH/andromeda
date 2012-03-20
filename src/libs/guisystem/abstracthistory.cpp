#include "abstracthistory.h"

using namespace GuiSystem;

/*!
  \class AbstractHistory

  \brief The AbstractHistory class is base class for Editor's history.
*/

/*!
  \brief Creates an AbstractHistory with the given \a parent.
*/
AbstractHistory::AbstractHistory(QObject *parent) :
    QObject(parent)
{
}

/*!
  \property AbstractHistory::canGoBack

  \brief This property holds whenever history can go  at least one item back.
*/
bool AbstractHistory::canGoBack() const
{
    return currentItemIndex() > 0;
}

/*!
  \brief Sets the current item to be the previous item in the history.
*/
void AbstractHistory::back()
{
    if (canGoBack())
        setCurrentItemIndex(currentItemIndex() - 1);
}

/*!
  \property AbstractHistory::canGoForward

  \brief This property holds whenever history can go at least one item forward.
*/
bool AbstractHistory::canGoForward() const
{
    return currentItemIndex() >= 0 && currentItemIndex() < count() - 1;
}

/*!
  \brief Sets the current item to be the next item in the history.
*/
void AbstractHistory::forward()
{
    if (canGoForward()) {
        setCurrentItemIndex(currentItemIndex() + 1);
    }
}

/*!
  \property AbstractHistory::currentItemIndex

  \brief This property holds current index in history.
*/

/*!
  \fn int AbstractHistory::count() const

  \brief Reimplement to return how many items this history contains.
*/

/*!
  \fn int AbstractHistory::currentItemIndex() const

  \brief Reimplement to return current index in history.

  Return -1 if history is empty.
*/

/*!
  \fn void AbstractHistory::setCurrentItemIndex(int index)

  \brief Reimplement to navigate to specified \a index in history.
*/

/*!
  \fn HistoryItem AbstractHistory::itemAt(int index) const

  \brief Reimplement to return item located at specified \a index.
*/

/*!
  \fn void AbstractHistory::clear()

  \brief Reimplement to clear this history.
*/

/*!
  \fn void AbstractHistory::canGoBackChanged(bool)

  \brief Emit this signal whenever history can go back or not.
*/

/*!
  \fn void AbstractHistory::canGoForwardChanged(bool)

  \brief Emit this signal whenever history can go forward or not.
*/

/*!
  \fn void AbstractHistory::currentItemIndexChanged(int index)

  \brief Emit this signal whenever current index becomes changed.
*/

/*!
  Emits currentItemIndexChanged signal for the given \a index and emits
  canGoBackChanged and canGoForwardChanged signals if necessary.
*/
void AbstractHistory::emitCurrentItemIndexChanged(int index, int oldIndex)
{
    emit currentItemIndexChanged(index);

    if (index == 0)
        emit canGoBackChanged(false);
    else if (oldIndex == 0)
        emit canGoBackChanged(true);
    if (index == count() - 1)
        emit canGoForwardChanged(false);
    else if (oldIndex == count() - 1)
        emit canGoForwardChanged(true);
}
