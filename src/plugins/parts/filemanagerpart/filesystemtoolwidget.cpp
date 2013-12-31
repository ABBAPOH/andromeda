#include "filesystemtoolwidget.h"
#include "filesystemtoolwidget_p.h"

#include <QtGui/QDesktopServices>
#include <QtGui/QKeyEvent>

#if QT_VERSION >= 0x050000
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMenu>
#include <QtWidgets/QVBoxLayout>
#else
#include <QtGui/QHeaderView>
#include <QtGui/QMenu>
#include <QtGui/QVBoxLayout>
#endif

#include <Parts/OpenStrategy>
#include <FileManager/FileSystemModel>

#include "filesystemtoolmodel.h"

using namespace Parts;
using namespace FileManager;

FileSystemToolWidget::TreeView::TreeView(QWidget * /*parent*/)
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

    OpenStrategy *strategy = OpenStrategy::defaultStrategy();
    if (!strategy)
        return;
    strategy->open(url);
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

    OpenStrategy *strategy = OpenStrategy::defaultStrategy();
    if (!strategy)
        return;
    strategy->open(url);
}

void FileSystemToolWidget::openStrategy()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (!action)
        return;

    QByteArray id = action->objectName().toUtf8();
    OpenStrategy *strategy = OpenStrategy::strategy(id);
    if (!strategy)
        return;
    QModelIndex index = m_view->currentIndex();
    if (!index.isValid())
        return;
    QString path = index.data(QFileSystemModel::FilePathRole).toString();
    QUrl url = QUrl::fromLocalFile(path);
    strategy->open(url);
}

void FileSystemToolWidget::contextMenuEvent(QContextMenuEvent *event)
{
    QModelIndex index = m_view->currentIndex();
    if (!index.isValid())
        return;

    QString path = index.data(QFileSystemModel::FilePathRole).toString();
    QUrl url = QUrl::fromLocalFile(path);

    QMenu menu;
    foreach (OpenStrategy *strategy, OpenStrategy::strategies()) {
        if (!strategy->canOpen(QList<QUrl>() << url))
            continue;
        QAction *action = new QAction(&menu);
        action->setToolTip(strategy->toolTip());
        action->setText(strategy->text());
        action->setShortcut(strategy->keySequence());
        action->setObjectName(strategy->id());
        connect(action, SIGNAL(triggered()), SLOT(openStrategy()));
        menu.addAction(action);
    }
    menu.exec(event->globalPos());
}
