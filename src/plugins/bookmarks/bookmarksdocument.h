#ifndef BOOKMARKSDOCUMENT_H
#define BOOKMARKSDOCUMENT_H

#include "bookmarks_global.h"

#include <GuiSystem/AbstractDocument>
#include <GuiSystem/AbstractDocumentFactory>

namespace Bookmarks {

class BookmarksModel;

class BOOKMARKS_EXPORT BookmarksDocument : public GuiSystem::AbstractDocument
{
    Q_OBJECT
    Q_DISABLE_COPY(BookmarksDocument)

public:
    explicit BookmarksDocument(QObject *parent = 0);

    BookmarksModel *model() const;

protected:
    bool openUrl(const QUrl &url);

protected:
    BookmarksModel *m_model;
};

class BookmarksDocumentFactory : public GuiSystem::AbstractDocumentFactory
{
    Q_OBJECT
    Q_DISABLE_COPY(BookmarksDocumentFactory)

public:
    explicit BookmarksDocumentFactory(QObject *parent = 0);

    QByteArray id() const;
    QString name() const;
    QIcon icon() const;

protected:
    GuiSystem::AbstractDocument *createDocument(QObject *parent);
};

} // namespace Bookmarks

#endif // BOOKMARKSDOCUMENT_H
