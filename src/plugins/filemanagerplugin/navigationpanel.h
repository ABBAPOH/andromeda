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

class NavigationPanelPrivate;
class NavigationPanel : public QWidget
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(NavigationPanel)
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

protected:
    NavigationPanelPrivate *d_ptr;
};

} // namespace FileManagerPlugin

#endif // QNAVIGATIONPANEL_H
