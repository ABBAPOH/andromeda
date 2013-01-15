#include "bookmarksdocument.h"

#include "bookmarksmodel.h"
#include "bookmarksplugin.h"

using namespace GuiSystem;
using namespace Bookmarks;

BookmarksDocument::BookmarksDocument(QObject *parent) :
    AbstractDocument(parent),
    m_model(new BookmarksModel(this))
{
    setIcon(QIcon(":/icons/bookmarks.png"));
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

BookmarksDocumentFactory::BookmarksDocumentFactory(QObject *parent)
{
}

/*!
    \reimp
*/
QByteArray BookmarksDocumentFactory::id() const
{
    return "bookmarks";
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
    return QIcon(":/icons/bookmarks.png");
}

/*!
    \reimp
*/
AbstractDocument * BookmarksDocumentFactory::createDocument(QObject *parent)
{
    return BookmarksPlugin::instance()->sharedDocument();
}
