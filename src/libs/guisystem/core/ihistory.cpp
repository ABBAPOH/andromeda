#include "ihistory.h"

using namespace GuiSystem;

/*!
  \class GuiSystem::IHistory

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
  \fn void IHistory::currentItemIndexChanged(int index)

  \brief Emit this signal whenever current index becomes changed.
*/
