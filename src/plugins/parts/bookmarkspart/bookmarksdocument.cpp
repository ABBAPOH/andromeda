#include "bookmarksdocument.h"

#include <Bookmarks/BookmarksModel>

#include "bookmarksplugin.h"
#include "bookmarksconstants.h"

using namespace Parts;
using namespace Bookmarks;

BookmarksDocument::BookmarksDocument(QObject *parent) :
    AbstractDocument(parent),
    m_model(new BookmarksModel(this))
{
    setIcon(QIcon(":/bookmarks/icons/bookmarks.png"));
    setTitle(tr("Bookmarks"));
}

BookmarksModel * BookmarksDocument::model() const
{
    return m_model;
}

bool BookmarksDocument::openUrl(const QUrl &/*url*/)
{
    return true;
}

/*!
    \class BookmarksDocumentFactory
*/

BookmarksDocumentFactory::BookmarksDocumentFactory(QObject *parent) :
    AbstractDocumentFactory(Constants::Editors::Bookmarks, parent)
{
}

/*!
    \reimp
*/
QString BookmarksDocumentFactory::name() const
{
    return tr("Bookmarks editor");
}

/*!
    \reimp
*/
QIcon BookmarksDocumentFactory::icon() const
{
    return QIcon(":/bookmarks/icons/bookmarks.png");
}

/*!
    \reimp
*/
AbstractDocument * BookmarksDocumentFactory::createDocument(QObject * /*parent*/)
{
    return BookmarksPlugin::instance()->sharedDocument();
}
