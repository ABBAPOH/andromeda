#include "navigationpanel.h"

#include "navigationmodel.h"

#include <QtGui/QTreeView>
#include <QtGui/QResizeEvent>

NavigationPanel::NavigationPanel(QWidget *parent) :
    QWidget(parent),
    m_treeView(new QTreeView(this)),
    m_model(new NavigationModel(this))
{
    m_treeView->setModel(m_model);
    m_treeView->setHeaderHidden(true);
    m_treeView->setFocusPolicy(Qt::NoFocus);

    QPalette pal = m_treeView->palette();
    pal.setColor(QPalette::Base, QColor(214, 221, 229));
    m_treeView->QAbstractItemView::setPalette(pal);
    m_treeView->expandAll();

    connect(m_treeView, SIGNAL(clicked(QModelIndex)), SLOT(onClick(QModelIndex)));

    setMinimumSize(200, 200);
}

NavigationPanel::~NavigationPanel()
{
}

void NavigationPanel::addFolder(const QString & path)
{
    m_model->addFolder(path);
}

void NavigationPanel::removeFolder(const QString &path)
{
    m_model->removeFolder(path);
}

void NavigationPanel::resizeEvent(QResizeEvent * event)
{
    m_treeView->resize(event->size());
    QWidget::resizeEvent(event);
}

void NavigationPanel::onClick(const QModelIndex &index)
{
    QString path = m_model->path(index);
    if (!path.isEmpty())
        emit folderClicked(path);
}
