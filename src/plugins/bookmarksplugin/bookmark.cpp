#include "bookmark.h"

#include <QtCore/QSharedData>

namespace Bookmarks {

class BookmarkData : public QSharedData
{
public:
    QString description;
    QIcon icon;
    QImage preview;
    QString title;
    QUrl url;
};

} // namespace Bookmarks

using namespace Bookmarks;

Bookmark::Bookmark() :
    d(new BookmarkData)
{
}

Bookmark::Bookmark(const Bookmark &other) :
    d(other.d)
{
}

Bookmark &Bookmark::operator=(const Bookmark &other)
{
    if (this != &other)
        d.operator=(other.d);
    return *this;
}

Bookmark::~Bookmark()
{
}

QString Bookmark::description() const
{
    return d->description;
}

void Bookmark::setDescription(const QString &newDescription)
{
    if (description() == newDescription)
        return;

    d->description = newDescription;
}

QIcon Bookmark::icon() const
{
    return d->icon;
}

void Bookmark::setIcon(const QIcon &icon)
{
    d->icon = icon;
}

QImage Bookmark::preview() const
{
    return d->preview;
}

void Bookmark::setPreview(const QImage &image)
{
    d->preview = image;
}

QString Bookmark::title() const
{
    return d->title;
}

void Bookmark::setTitle(const QString &newTitle)
{
    if (title() == newTitle)
        return;

    d->title = newTitle;
}

QUrl Bookmark::url() const
{
    return d->url;
}

void Bookmark::setUrl(const QUrl &newUrl)
{
    if (url() == newUrl)
        return;

    d->url = newUrl;
}

QDataStream & Bookmarks::operator>>(QDataStream &s, Bookmark &b)
{
    s >> b.d->title;
    s >> b.d->url;
    s >> b.d->description;
#ifdef Q_OS_LINUX
    QPixmap pix;
    s >> pix;
    b.d->icon = pix;
#else
    s >> b.d->icon;
#endif
    s >> b.d->preview;
    return s;
}

QDataStream & Bookmarks::operator<<(QDataStream &s, const Bookmark &b)
{
    s << b.d->title;
    s << b.d->url;
    s << b.d->description;
#ifdef Q_OS_LINUX
    s << b.d->icon.pixmap(32);
#else
    s << b.d->icon;
#endif
    s << b.d->preview;
    return s;
}
