#include "filesystemtoolwidget.h"
#include "filesystemtoolwidget_p.h"

#include <QtGui/QDesktopServices>
#include <QtGui/QHeaderView>
#include <QtGui/QKeyEvent>
#include <QtGui/QMenu>
#include <QtGui/QVBoxLayout>
#include <GuiSystem/EditorWindowFactory>
#include <FileManager/FileSystemModel>

#include "filesystemtoolmodel.h"

using namespace GuiSystem;
using namespace FileManager;

FileSystemToolWidget::TreeView::TreeView(QWidget *parent)
{
    setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void FileSystemToolWidget::TreeView::keyPressEvent(QKeyEvent *event)
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

FileSystemToolWidget::FileSystemToolWidget(QWidget *parent) :
    ToolWidget(*new FileSystemToolModel, parent)
{
    FileSystemToolModel *model = static_cast<FileSystemToolModel *>(this->model());
    model->setParent(this);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    m_view = new TreeView(this);
    layout->addWidget(m_view);

    m_view->header()->hide();
    m_view->setModel(model->fileSystemModel());
    m_view->hideColumn(1);
    m_view->hideColumn(2);
    m_view->hideColumn(3);
    m_view->expandAll();
    m_view->setDragDropMode(QAbstractItemView::DropOnly);
    connect(m_view, SIGNAL(clicked(QModelIndex)),
            this, SLOT(onActivated(QModelIndex)));
    connect(m_view, SIGNAL(triggered(QModelIndex)),
            this, SLOT(onActivated(QModelIndex)));
    connect(m_view, SIGNAL(doubleClicked(QModelIndex)),
            this, SLOT(open()));

    connect(model, SIGNAL(urlChanged(QUrl)), SLOT(onUrlChanged(QUrl)));
}

void FileSystemToolWidget::onActivated(const QModelIndex &index)
{
    const FileSystemModel *model = qobject_cast<const FileSystemModel *>(index.model());

    QString path = index.data(QFileSystemModel::FilePathRole).toString();
    QUrl url = QUrl::fromLocalFile(path);

    if (!model->isDir(index))
        return;

    EditorWindowFactory *factory = EditorWindowFactory::defaultFactory();
    if (factory)
        factory->open(url);
}

void FileSystemToolWidget::onUrlChanged(const QUrl &url)
{
    if (!url.isLocalFile())
        return;

    FileSystemToolModel *model = static_cast<FileSystemToolModel *>(this->model());
    FileSystemModel *fileSystemModel = model->fileSystemModel();
    QString path = url.toLocalFile();
    QModelIndex index = fileSystemModel->index(path);
    if (m_view->currentIndex() == index)
        return;

    m_view->collapseAll();
    m_view->setCurrentIndex(index);
    if (fileSystemModel->isDir(index))
        m_view->expand(index);
}

void FileSystemToolWidget::open()
{
    QModelIndex index = m_view->currentIndex();
    const FileSystemModel *model = qobject_cast<const FileSystemModel *>(index.model());
    QString path = index.data(QFileSystemModel::FilePathRole).toString();
    QUrl url = QUrl::fromLocalFile(path);

    if (!model->isDir(index)) {
        QDesktopServices::openUrl(url);
        return;
    }

    EditorWindowFactory *factory = EditorWindowFactory::defaultFactory();
    if (factory)
        factory->open(url);
}

void FileSystemToolWidget::openInTab()
{
    QModelIndex index = m_view->currentIndex();
    QString path = index.data(QFileSystemModel::FilePathRole).toString();
    QUrl url = QUrl::fromLocalFile(path);

    EditorWindowFactory *factory = EditorWindowFactory::defaultFactory();
    if (factory)
        factory->openNewEditor(url);
}

void FileSystemToolWidget::openInWindow()
{
    QModelIndex index = m_view->currentIndex();
    QString path = index.data(QFileSystemModel::FilePathRole).toString();
    QUrl url = QUrl::fromLocalFile(path);

    EditorWindowFactory *factory = EditorWindowFactory::defaultFactory();
    if (factory)
        factory->openNewWindow(url);
}

void FileSystemToolWidget::contextMenuEvent(QContextMenuEvent *event)
{
    QModelIndex index = m_view->currentIndex();
    if (!index.isValid())
        return;
    QMenu menu;
    menu.addAction(tr("Open"), this, SLOT(open()));
    menu.addAction(tr("Open in new tab"), this, SLOT(openInTab()));
    menu.addAction(tr("Open in new window"), this, SLOT(openInWindow()));
    menu.exec(event->globalPos());
}
