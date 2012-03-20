#include "ihistory.h"

using namespace GuiSystem;

/*!
  \class IHistory

  \brief The IHistory class is base class for Editor's history.
*/

/*!
  \brief Creates an IHistory with the given \a parent.
*/
IHistory::IHistory(QObject *parent) :
    QObject(parent)
{
}

/*!
  \property IHistory::canGoBack

  \brief This property holds whenever history can go  at least one item back.
*/
bool IHistory::canGoBack() const
{
    return currentItemIndex() > 0;
}

/*!
  \brief Sets the current item to be the previous item in the history.
*/
void IHistory::back()
{
    if (canGoBack())
        setCurrentItemIndex(currentItemIndex() - 1);
}

/*!
  \property IHistory::canGoForward

  \brief This property holds whenever history can go at least one item forward.
*/
bool IHistory::canGoForward() const
{
    return currentItemIndex() >= 0 && currentItemIndex() < count() - 1;
}

/*!
  \brief Sets the current item to be the next item in the history.
*/
void IHistory::forward()
{
    if (canGoForward()) {
        setCurrentItemIndex(currentItemIndex() + 1);
    }
}

/*!
  \property IHistory::currentItemIndex

  \brief This property holds current index in history.
*/

/*!
  \fn int IHistory::count() const

  \brief Reimplement to return how many items this history contains.
*/

/*!
  \fn int IHistory::currentItemIndex() const

  \brief Reimplement to return current index in history.

  Return -1 if history is empty.
*/

/*!
  \fn void IHistory::setCurrentItemIndex(int index)

  \brief Reimplement to navigate to specified \a index in history.
*/

/*!
  \fn HistoryItem IHistory::itemAt(int index) const

  \brief Reimplement to return item located at specified \a index.
*/

/*!
  \fn void IHistory::clear()

  \brief Reimplement to clear this history.
*/

/*!
  \fn void IHistory::canGoBackChanged(bool)

  \brief Emit this signal whenever history can go back or not.
*/

/*!
  \fn void IHistory::canGoForwardChanged(bool)

  \brief Emit this signal whenever history can go forward or not.
*/

/*!
  \fn void IHistory::currentItemIndexChanged(int index)

  \brief Emit this signal whenever current index becomes changed.
*/

/*!
  Emits currentItemIndexChanged signal for the given \a index and emits
  canGoBackChanged and canGoForwardChanged signals if necessary.
*/
void IHistory::emitCurrentItemIndexChanged(int index, int oldIndex)
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
