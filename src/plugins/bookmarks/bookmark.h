#ifndef BOOKMARK_H
#define BOOKMARK_H

#include "bookmarks_global.h"

#include <QtCore/QSharedDataPointer>
#include <QtCore/QUrl>
#include <QtGui/QIcon>

namespace Bookmarks {

class BookmarkData;

class BOOKMARKS_EXPORT Bookmark
{
public:
    Bookmark();
    Bookmark(const Bookmark &);
    Bookmark &operator=(const Bookmark &);
    ~Bookmark();

    QString description() const;
    void setDescription(const QString &description);

    QIcon icon() const;
    void setIcon(const QIcon &icon);

    QImage preview() const;
    void setPreview(const QImage &image);

    QString title() const;
    void setTitle(const QString &title);

    QUrl url() const;
    void setUrl(const QUrl &url);

private:
    QSharedDataPointer<BookmarkData> d;

    friend QDataStream & operator>>(QDataStream &s, Bookmark &b);
    friend QDataStream & operator<<(QDataStream &s, const Bookmark &b);
};

QDataStream & operator>>(QDataStream &s, Bookmark &b);
QDataStream & operator<<(QDataStream &s, const Bookmark &b);

} // namespace Bookmarks

#endif // BOOKMARK_H
