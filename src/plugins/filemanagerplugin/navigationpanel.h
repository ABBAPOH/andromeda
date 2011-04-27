#ifndef QNAVIGATIONPANEL_H
#define QNAVIGATIONPANEL_H

#include <QWidget>
#include <QHash>

class NavigationModel;
class QModelIndex;
class QTreeView;
class QTreeWidget;
class QTreeWidgetItem;

namespace FileManagerPlugin {

class NavigationPanel : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QString currentPath READ currentPath NOTIFY currentPathChanged)
public:
    explicit NavigationPanel(QWidget *parent = 0);
    ~NavigationPanel();

    void addFolder(const QString & path);
    void removeFolder(const QString & path);

    QString currentPath() const;

protected:
    virtual void resizeEvent(QResizeEvent *);

signals:
    void currentPathChanged(const QString & path);

private slots:
    void onClick(const QModelIndex &index);

private:
    QTreeView *m_treeView;
    NavigationModel *m_model;
    QString m_currentPath;
};

} // namespace FileManagerPlugin

#endif // QNAVIGATIONPANEL_H
