#include "bookmarkstoolmodel.h"

#include "bookmarksdocument.h"
#include "bookmarksplugin.h"

using namespace Bookmarks;

BookmarksToolModel::BookmarksToolModel(QObject *parent) :
    ToolModel(parent),
    m_model(BookmarksPlugin::instance()->sharedDocument()->model())
{
    setTitle(tr("Bookmarks"));
}

BookmarksModel * BookmarksToolModel::bookmarksModel() const
{
    return m_model;
}
