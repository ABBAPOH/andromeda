#ifndef BOOKMARKSDOCUMENT_H
#define BOOKMARKSDOCUMENT_H

#include <Parts/AbstractDocument>
#include <Parts/AbstractDocumentFactory>

namespace Bookmarks {

class BookmarksModel;

class BookmarksDocument : public Parts::AbstractDocument
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

class BookmarksDocumentFactory : public Parts::AbstractDocumentFactory
{
    Q_OBJECT
    Q_DISABLE_COPY(BookmarksDocumentFactory)

public:
    explicit BookmarksDocumentFactory(QObject *parent = 0);

    QString name() const;
    QIcon icon() const;

protected:
    Parts::AbstractDocument *createDocument(QObject *parent);
};

} // namespace Bookmarks

#endif // BOOKMARKSDOCUMENT_H
