#ifndef BOOKMARKDIALOG_H
#define BOOKMARKDIALOG_H

#include "bookmarks_global.h"

#include <QtGui/QDialog>

class QModelIndex;

namespace Bookmarks {

class BookmarksModel;

class BookmarkDialogPrivate;
class BOOKMARKS_EXPORT BookmarkDialog : public QDialog
{
    Q_OBJECT
    Q_DISABLE_COPY(BookmarkDialog)

    Q_PROPERTY(bool isFolder READ isFolder WRITE setFolder)
    Q_PROPERTY(QString title READ title WRITE setTitle)
    Q_PROPERTY(QString url READ url WRITE setUrl)

public:
    explicit BookmarkDialog(QWidget *parent = 0);
    ~BookmarkDialog();

    bool isFolder() const;
    void setFolder(bool isFolder);

    bool isUrlShown() const;
    void showUrl(bool show);

    QString description() const;
    void setDescription(const QString &description);

    QIcon icon() const;
    void setIcon(const QIcon &icon);

    QImage preview() const;
    void setPreview(const QImage &image);

    QString title() const;
    void setTitle(const QString &title);

    QString url() const;
    void setUrl(const QString &url);

    BookmarksModel *model() const;
    void setModel(BookmarksModel *model);

    QModelIndex currentIndex() const;
    void setCurrentIndex(const QModelIndex &index);

    QModelIndex addedIndex() const;

    void accept();

private slots:
    void onIndexChanged(int);

private:
    BookmarkDialogPrivate *d;
};

} // namespace Bookmarks

#endif // BOOKMARKDIALOG_H
