#include "bookmarksdocument.h"

using namespace GuiSystem;
using namespace Bookmarks;

BookmarksDocument::BookmarksDocument(QObject *parent) :
    AbstractDocument(parent)
{
    setIcon(QIcon(":/icons/bookmarks.png"));
    setTitle(tr("Bookmarks"));
}
