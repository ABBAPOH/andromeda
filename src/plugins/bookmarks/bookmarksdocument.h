#ifndef BOOKMARKSDOCUMENT_H
#define BOOKMARKSDOCUMENT_H

#include "bookmarks_global.h"

#include <GuiSystem/AbstractDocument>

namespace Bookmarks {

class BookmarksModel;

class BOOKMARKS_EXPORT BookmarksDocument : public GuiSystem::AbstractDocument
{
    Q_OBJECT
public:
    explicit BookmarksDocument(QObject *parent = 0);

    BookmarksModel *model() const;

protected:
    bool openUrl(const QUrl &url);

protected:
    BookmarksModel *m_model;
};

} // namespace Bookmarks

#endif // BOOKMARKSDOCUMENT_H
