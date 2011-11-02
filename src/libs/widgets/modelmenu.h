#ifndef MODELMENU_H
#define MODELMENU_H

#include <QtGui/QMenu>

class QAbstractItemModel;
class QModelIndex;

// A QMenu that is dynamically populated from a QAbstractItemModel
class ModelMenuPrivate;
class ModelMenu : public QMenu
{
    Q_OBJECT
    Q_DISABLE_COPY(ModelMenu)

public:
    explicit ModelMenu(QWidget *parent = 0);
    ~ModelMenu();

    QAbstractItemModel *model() const;
    void setModel(QAbstractItemModel *model);

    int maxRows() const;
    void setMaxRows(int max);

    int firstSeparator() const;
    void setFirstSeparator(int offset);

    QModelIndex rootIndex() const;
    void setRootIndex(const QModelIndex &index);

    int statusBarTextRole() const;
    void setStatusBarTextRole(int role);

    int separatorRole() const;
    void setSeparatorRole(int role);

    QAction *makeAction(const QIcon &icon, const QString &text, QObject *parent);
    QModelIndex index(QAction *action);

signals:
    void activated(const QModelIndex &index);

protected:
    // add any actions before the tree, return true if any actions are added.
    virtual bool prePopulated();
    // add any actions after the tree
    virtual void postPopulated();
    // return the QMenu that is used to populate sub menu's
    virtual ModelMenu *createBaseMenu();

    // put all of the children of parent into menu up to max
    void createMenu(const QModelIndex &parent, int max, QMenu *parentMenu = 0, QMenu *menu = 0);

    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);

    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

private slots:
    void aboutToShow();
    void actionTriggered(QAction *action);

private:
    QAction *makeAction(const QModelIndex &index);

private:
    ModelMenuPrivate *d;
};

#endif // MODELMENU_H

