#ifndef BOOKMARKSPLUGIN_P_H
#define BOOKMARKSPLUGIN_P_H

#include "bookmarksplugin.h"

#include <Parts/ModelContainer>

#include <Bookmarks/BookmarksMenu>

namespace Bookmarks {

class BookmarksMenuContainer : public Parts::CommandContainer
{
public:
    explicit BookmarksMenuContainer(const QByteArray &id, QObject *parent = 0) :
        CommandContainer(id, parent),
        m_menu(new BookmarksMenuBarMenu)
    {}

    ~BookmarksMenuContainer() { delete m_menu; }

    QMenu *createMenu(QWidget * = 0) const { return m_menu; }
    inline BookmarksMenuBarMenu *bookmarksMenu() const { return m_menu; }

private:
    BookmarksMenuBarMenu *m_menu;
};

class BookmarksButtonCommand : public Parts::AbstractCommand
{
public:
    explicit BookmarksButtonCommand(QObject *parent = 0);

    QAction *createAction(QObject *parent) const;
};

class BookmarksToolBarContainer : public Parts::ModelContainer
{
public:
    explicit BookmarksToolBarContainer(const QByteArray &id, QObject *parent = 0);
};

} // namespace Bookmarks

#endif // BOOKMARKSPLUGIN_P_H
