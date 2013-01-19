#ifndef BOOKMARKSTOOLMODEL_H
#define BOOKMARKSTOOLMODEL_H

#include <GuiSystem/ToolModel>

namespace Bookmarks {

class BookmarksModel;

class BookmarksToolModel : public GuiSystem::ToolModel
{
    Q_OBJECT
    Q_DISABLE_COPY(BookmarksToolModel)

public:
    explicit BookmarksToolModel(QObject *parent = 0);

    BookmarksModel *bookmarksModel() const;

protected:
    BookmarksModel *m_model;
};

} // namespace Bookmarks

#endif // BOOKMARKSTOOLMODEL_H
