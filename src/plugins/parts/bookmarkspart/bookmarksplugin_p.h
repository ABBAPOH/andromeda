#ifndef BOOKMARKSPLUGIN_P_H
#define BOOKMARKSPLUGIN_P_H

#include "bookmarksplugin.h"

namespace Bookmarks {

class BookmarksButtonCommand : public Parts::AbstractCommand
{
public:
    explicit BookmarksButtonCommand(QObject *parent = 0);

    QAction *createAction(QObject *parent) const;
};

} // namespace Bookmarks

#endif // BOOKMARKSPLUGIN_P_H
