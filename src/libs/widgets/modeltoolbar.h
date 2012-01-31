#ifndef MODELTOOLBAR_H
#define MODELTOOLBAR_H

#include "widgets_global.h"

#include <QtGui/QToolBar>

class ModelMenu;
class QAbstractItemModel;
class QEvent;
class QModelIndex;

class ModelToolBarPrivate;
class WIDGETS_EXPORT ModelToolBar : public QToolBar
{
    Q_OBJECT
    Q_DISABLE_COPY(ModelToolBar)

    Q_PROPERTY(bool showIcons READ showIcons WRITE setShowIcons)

public:
    explicit ModelToolBar(QWidget *parent = 0);
    explicit ModelToolBar(const QString &title, QWidget *parent = 0);
    ~ModelToolBar();

    bool showIcons() const;
    void setShowIcons(bool show);

    virtual void setModel(QAbstractItemModel *model);
    QAbstractItemModel *model() const;

    void setRootIndex(const QModelIndex &index);
    QModelIndex rootIndex() const;

    static QModelIndex index(QAction *action);

signals:
    void activated(const QModelIndex &index);

protected:
    virtual ModelMenu *createMenu();
    virtual void prePopulated();
    virtual void postPopulated();

    bool eventFilter(QObject *object, QEvent *event);

    void hideEvent(QHideEvent *event);
    void showEvent(QShowEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

protected slots:
    void build();

private:
    ModelToolBarPrivate *d;
};

#endif // MODELTOOLBAR_H
