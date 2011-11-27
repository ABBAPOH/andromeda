#include "ifind.h"

using namespace GuiSystem;

/*!
  \class IFind

  \brief The IFind class is a basic interface class for all find imlementations.
*/

/*!
  \brief Creates an IFind class with the given \a parent.
*/
IFind::IFind(QObject *parent) :
    QObject(parent)
{
}

/*!
  \brief Destroys IFind object.
*/
IFind::~IFind()
{
}

/*!
  \brief Return true if editor can replace found entries.

  Default implementation returns false.
*/
bool IFind::supportsReplace() const
{
    return false;
}

/*!
  \fn IFind::FindFlags IFind::supportedFindFlags() const

  \brief Return find flags that editor can support.
*/

/*!
  \brief Reimplement to reset incremental search.
*/
void IFind::resetIncrementalSearch()
{
}

/*!
  \fn void IFind::clearResults()

  \brief Reimplement to clear found results.
*/

/*!
  \fn QString IFind::currentFindString() const

  \brief Return last found string.
*/

/*!
  \fn QString IFind::completedFindString() const

  \brief Return last found string completed to whole word.
*/

/*!
  \brief Reimplement to highligh all entries of \a txt in editor.
*/
void IFind::highlightAll(const QString &, IFind::FindFlags)
{
}

/*!
  \fn IFind::Result IFind::findIncremental(const QString &txt, IFind::FindFlags findFlags)

  \brief Reimplement to perform incremental search of string \a txt.
*/

/*!
  \fn IFind::Result IFind::findStep(const QString &txt, IFind::FindFlags findFlags)

  \brief Reimplement to perform jumping to next occurence of string \a txt.
*/

/*!
  \brief Reimplement to replace current occurrence of string \a before with
  \a after in the opened document.

  Default implementation does nothing.
*/
void IFind::replace(const QString &, const QString &, IFind::FindFlags)
{
}

/*!
  \brief Reimplement to replace current occurrence of string \a before with
  \a after in the opened document and move to next occurence.

  Return true if jump to next occurence was successful.

  Default implementation does nothing and returns false.
*/
bool IFind::replaceStep(const QString &, const QString &, IFind::FindFlags)
{
    return false;
}

/*!
  \brief Reimplement to replace all occurrences of string \a before with \a after in the opened document.

  Return number of replaced entries.

  Default implementation does nothing and returns 0.
*/
int IFind::replaceAll(const QString &before, const QString &after, IFind::FindFlags findFlags)
{
    return 0;
}
