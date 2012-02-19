#include "bookmarkdialog.h"

#include <QtGui/QComboBox>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QSpacerItem>
#include <QtGui/QTreeView>
#include <QtGui/QVBoxLayout>

#include "bookmarkswidget_p.h"

namespace Bookmarks {

class BookmarkDialogPrivate
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *label;
    QLineEdit *title;
    QLineEdit *url;
    QLineEdit *description;
    QComboBox *location;
    QTreeView *treeView;
    QSpacerItem *verticalSpacer;
    QDialogButtonBox *buttonBox;

    QIcon icon;
    QImage preview;

    bool isFolder;
    FolderProxyModel *proxy;

    QPersistentModelIndex currentIndex;
    QPersistentModelIndex addedIndex;

    void setupUi(BookmarkDialog *);
    void retranslateUi(BookmarkDialog *);
};

} // namespace Bookmarks

using namespace Bookmarks;

void BookmarkDialogPrivate::setupUi(BookmarkDialog *bookmarkDialog)
{
    if (bookmarkDialog->objectName().isEmpty())
        bookmarkDialog->setObjectName(QLatin1String("AddBookmarkDialog"));
    bookmarkDialog->setWindowFlags(Qt::Sheet);
//    bookmarkDialog->resize(240, 180);

    verticalLayout = new QVBoxLayout(bookmarkDialog);
    verticalLayout->setObjectName(QLatin1String("verticalLayout"));

    label = new QLabel(bookmarkDialog);
    label->setObjectName(QLatin1String("label"));
    label->setTextFormat(Qt::PlainText);
    label->setWordWrap(true);

    verticalLayout->addWidget(label);

    url = new QLineEdit(bookmarkDialog);
    url->setObjectName(QLatin1String("url"));
    verticalLayout->addWidget(url);

    title = new QLineEdit(bookmarkDialog);
    title->setObjectName(QLatin1String("title"));
    verticalLayout->addWidget(title);

    description = new QLineEdit(bookmarkDialog);
    description->setObjectName(QLatin1String("description"));
    verticalLayout->addWidget(description);

    treeView = new QTreeView(bookmarkDialog);
    treeView->setModel(proxy);
    treeView->expandAll();
    treeView->header()->setStretchLastSection(true);
    treeView->header()->hide();
    treeView->setItemsExpandable(false);
    treeView->setRootIsDecorated(false);
    treeView->setIndentation(10);
    treeView->show();

    location = new QComboBox(bookmarkDialog);
    location->setObjectName(QLatin1String("location"));
    location->setModel(proxy);
    location->setView(treeView);

    verticalLayout->addWidget(location);

    verticalSpacer = new QSpacerItem(20, 2, QSizePolicy::Minimum, QSizePolicy::Expanding);

    verticalLayout->addItem(verticalSpacer);

    buttonBox = new QDialogButtonBox(bookmarkDialog);
    buttonBox->setObjectName(QLatin1String("buttonBox"));
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
    buttonBox->setCenterButtons(false);

    verticalLayout->addWidget(buttonBox);

    retranslateUi(bookmarkDialog);

    QObject::connect(location, SIGNAL(currentIndexChanged(int)), bookmarkDialog, SLOT(onIndexChanged(int)));
    QObject::connect(buttonBox, SIGNAL(accepted()), bookmarkDialog, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), bookmarkDialog, SLOT(reject()));
} // setupUi

void BookmarkDialogPrivate::retranslateUi(BookmarkDialog *bookmarkDialog)
{
    bookmarkDialog->setWindowTitle(BookmarkDialog::tr("Add Bookmark"));
    label->setText(BookmarkDialog::tr("Type a name for the bookmark, and choose where to keep it."));
    title->setPlaceholderText(BookmarkDialog::tr("Title"));
    url->setPlaceholderText(BookmarkDialog::tr("Url"));
    description->setPlaceholderText(BookmarkDialog::tr("Description"));
} // retranslateUi

BookmarkDialog::BookmarkDialog(QWidget *parent) :
    QDialog(parent),
    d(new BookmarkDialogPrivate)
{
    d->isFolder = false;
    d->proxy = new FolderProxyModel(this);
    d->setupUi(this);
}

BookmarkDialog::~BookmarkDialog()
{
    delete d;
}

bool BookmarkDialog::isFolder() const
{
    return d->isFolder;
}

void BookmarkDialog::setFolder(bool isFolder)
{
    d->isFolder = isFolder;

    if (isFolder) {
        setWindowTitle(tr("Add Folder"));
        d->url->setVisible(false);
        d->description->setVisible(false);
    } else {
        setWindowTitle(tr("Add Bookmark"));
        d->url->setVisible(true);
        d->description->setVisible(true);
    }

    resize(sizeHint());
}

bool BookmarkDialog::isUrlShown() const
{
    return !d->url->isHidden();
}

void BookmarkDialog::showUrl(bool show)
{
    d->url->setVisible(show);
}

QString BookmarkDialog::description() const
{
    return d->description->text();
}

void BookmarkDialog::setDescription(const QString &description)
{
    d->description->setText(description);
}

QIcon BookmarkDialog::icon() const
{
    return d->icon;
}

void BookmarkDialog::setIcon(const QIcon &icon)
{
    d->icon = icon;
}

QImage BookmarkDialog::preview() const
{
    return d->preview;
}

void BookmarkDialog::setPreview(const QImage &image)
{
    d->preview = image;
}

void BookmarkDialog::setTitle(const QString &title)
{
    d->title->setText(title);
}

QString BookmarkDialog::title() const
{
    return d->title->text();
}

void BookmarkDialog::setUrl(const QString &url)
{
    d->url->setText(url);
    resize(sizeHint());
}

QString BookmarkDialog::url() const
{
    return d->url->text();
}

BookmarksModel * BookmarkDialog::model() const
{
    return qobject_cast<BookmarksModel*>(d->proxy->sourceModel());
}

void BookmarkDialog::setModel(BookmarksModel *model)
{
    d->proxy->setSourceModel(model);
    setCurrentIndex(model->index(0, 0));
    d->treeView->expandAll();
}

void BookmarkDialog::setCurrentIndex(const QModelIndex &index)
{
    if (!index.isValid())
        return;

    QModelIndex proxyIndex = d->proxy->mapFromSource(index);
    d->treeView->setCurrentIndex(proxyIndex);
    d->location->setCurrentIndex(proxyIndex.row());
    d->currentIndex = index;
}

QModelIndex BookmarkDialog::currentIndex() const
{
    return d->currentIndex;
}

QModelIndex BookmarkDialog::addedIndex() const
{
    return d->addedIndex;
}

void BookmarkDialog::accept()
{
    if ((!d->isFolder && d->url->text().isEmpty()) || d->title->text().isEmpty() || !model()) {
        QDialog::accept();
        return;
    }

    QModelIndex index = currentIndex();
    if (!index.isValid())
        index = model()->index(0, 0);

    if (d->isFolder) {
        d->addedIndex = model()->addFolder(title(), index);
    } else {
        Bookmark b;
        b.setDescription(description());
        b.setIcon(icon());
        b.setUrl(QUrl(url()));
        b.setPreview(preview());
        b.setTitle(title());
        d->addedIndex = model()->addBookmark(b, index);
    }

    QDialog::accept();
}

void BookmarkDialog::onIndexChanged(int)
{
    QModelIndex index = d->location->view()->currentIndex();
    d->currentIndex = d->proxy->mapToSource(index);
}
