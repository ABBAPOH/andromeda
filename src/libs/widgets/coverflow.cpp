#include "coverflow.h"

#include <QResizeEvent>

class CoverFlowTreeView : public QTreeView
{
public:
    CoverFlowTreeView(QWidget *parent) : QTreeView(parent) {}
    CoverFlow *coverFlow;

    void setModel(QAbstractItemModel *model)
    {
        if (coverFlow->model() != model) {
            QTreeView::setModel(model);
            coverFlow->setModel(model);
        } else {
            QTreeView::setModel(model);
        }
    }

    void setRootIndex(const QModelIndex &index)
    {
        if (coverFlow->rootIndex() != index) {
            QTreeView::setRootIndex(index);
            coverFlow->setRootIndex(index);
        } else {
            QTreeView::setRootIndex(index);
        }
    }
};

CoverFlow::CoverFlow(QWidget *parent) :
    QWidget(parent),
    m_splitter(new MiniSplitter(this)),
    m_flowView(new QxtFlowView(m_splitter)),
    m_treeView(new CoverFlowTreeView(m_splitter))
{
    ((CoverFlowTreeView*)m_treeView)->coverFlow = this;
    m_splitter->setOrientation(Qt::Vertical);
    m_splitter->addWidget(m_flowView);
    m_splitter->addWidget(m_treeView);
    resize(800, 600);
    m_splitter->setSizes(QList<int>() << 400 << 400);
}

QAbstractItemModel * CoverFlow::model() const
{
    return m_treeView->model();
}

void CoverFlow::setModel(QAbstractItemModel *model)
{
    if (this->model()) {
        disconnect(m_treeView->selectionModel(), 0, m_flowView, 0);
        disconnect(m_flowView, 0, m_treeView, 0);
    }

    m_flowView->setModel(model);
    m_treeView->setModel(model);
    connect(m_treeView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            m_flowView, SLOT(setCurrentIndex(QModelIndex)));
    connect(m_flowView, SIGNAL(currentIndexChanged(QModelIndex)),
            m_treeView, SLOT(setCurrentIndex(QModelIndex)));
}

QModelIndex CoverFlow::rootIndex() const
{
    return m_treeView->rootIndex();
}

void CoverFlow::setRootIndex(const QModelIndex &index)
{
    m_flowView->setRootIndex(index);
    m_treeView->setRootIndex(index);
}

int CoverFlow::pictureRole()
{
    return m_flowView->pictureRole();
}

void CoverFlow::setPictureRole(int role)
{
    m_flowView->setPictureRole(role);
}

int CoverFlow::pictureColumn()
{
    return m_flowView->pictureColumn();
}

void CoverFlow::setPictureColumn(int column)
{
    m_flowView->setPictureColumn(column);
}

QTreeView * CoverFlow::treeView() const
{
    return m_treeView;
}

void CoverFlow::onCurrentIndexChanged(const QModelIndex &index)
{
    m_flowView->setCurrentIndex(index);
}

void CoverFlow::resizeEvent(QResizeEvent *event)
{
    m_splitter->resize(event->size());
}

