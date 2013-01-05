#ifndef BOOKMARKSDOCUMENT_H
#define BOOKMARKSDOCUMENT_H

#include "bookmarks_global.h"

#include <guisystem/abstractdocument.h>

namespace Bookmarks {

class BOOKMARKS_EXPORT BookmarksDocument : public GuiSystem::AbstractDocument
{
    Q_OBJECT
public:
    explicit BookmarksDocument(QObject *parent = 0);

protected:
    bool openUrl(const QUrl &url) { return true; }
};

} // namespace Bookmarks

#endif // BOOKMARKSDOCUMENT_H
