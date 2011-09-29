#ifndef COVERFLOW_H
#define COVERFLOW_H

#include "widgets_global.h"

#include <QtGui/QTreeView>

#include "qxtflowview.h"
#include "minisplitter.h"

class WIDGETS_EXPORT CoverFlow : public QWidget
{
    Q_OBJECT
public:
    explicit CoverFlow(QWidget *parent = 0);

    QAbstractItemModel *model() const;
    void setModel(QAbstractItemModel *model);

    QModelIndex rootIndex() const;
    void setRootIndex(const QModelIndex &index);

    int pictureRole();
    void setPictureRole(int);

    int pictureColumn();
    void setPictureColumn(int);

    QTreeView *treeView() const;

protected:
    void resizeEvent(QResizeEvent *event);

protected slots:
    void onCurrentIndexChanged(const QModelIndex &index);

protected:
    MiniSplitter *m_splitter;
    QxtFlowView *m_flowView;
    QTreeView *m_treeView;
};

#endif // COVERFLOW_H
