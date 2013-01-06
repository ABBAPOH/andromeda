#include "bookmarksdocument.h"

#include "bookmarksmodel.h"

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
