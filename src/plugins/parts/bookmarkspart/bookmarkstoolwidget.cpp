#include "bookmarkstoolwidget.h"
#include "bookmarkstoolwidget_p.h"

#include <QtGui/QKeyEvent>

#if QT_VERSION >= 0x050000
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QVBoxLayout>
#else
#include <QtGui/QHeaderView>
#include <QtGui/QVBoxLayout>
#endif

#include <Parts/EditorWindowFactory>
#include <Parts/ToolModel>

#include "bookmarksmodel.h"
#include "bookmarksplugin.h"
#include "bookmarkstoolmodel.h"

using namespace Parts;
using namespace Bookmarks;

Bookmarks::BookmarksToolWidget::TreeView::TreeView(QWidget * parent) :
    QTreeView(parent)
{
    setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void BookmarksToolWidget::TreeView::keyPressEvent(QKeyEvent *event)
{
    QModelIndex index = currentIndex();
    switch (event->key()) {
    case Qt::Key_Enter:
    case Qt::Key_Return:
        emit triggered(index);
        return;
    case Qt::Key_Down:
        if (event->modifiers() & Qt::ControlModifier) {
            emit triggered(index);
            return;
        }
    default:
        break;
    }

    QTreeView::keyPressEvent(event);
}

BookmarksToolWidget::BookmarksToolWidget(QWidget *parent) :
    ToolWidget(*new BookmarksToolModel, parent)
{
    BookmarksToolModel *model = static_cast<BookmarksToolModel *>(this->model());
    model->setParent(this);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    m_view = new TreeView(this);
    layout->addWidget(m_view);

    m_view->header()->hide();
    m_view->setModel(model->bookmarksModel());
    m_view->hideColumn(1);
    m_view->hideColumn(2);
    m_view->expandAll();
    connect(m_view, SIGNAL(clicked(QModelIndex)),
            this, SLOT(onActivated(QModelIndex)));
    connect(m_view, SIGNAL(triggered(QModelIndex)),
            this, SLOT(onActivated(QModelIndex)));
}

void BookmarksToolWidget::onActivated(const QModelIndex &index)
{
    const BookmarksModel *model = qobject_cast<const BookmarksModel *>(index.model());
    if (model->isFolder(index))
        return;

    QUrl url = index.data(BookmarksModel::UrlRole).toUrl();

    EditorWindowFactory *factory = EditorWindowFactory::defaultFactory();
    if (factory)
        factory->open(url);
}

