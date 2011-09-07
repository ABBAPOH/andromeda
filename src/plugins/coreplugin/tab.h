#ifndef TAB_H
#define TAB_H

#include "coreplugin_global.h"

#include <QtGui/QWidget>
#include <perspectivewidget.h>

#include "history.h"

namespace CorePlugin {

class IEditor;

class TabPrivate;
class COREPLUGIN_EXPORT Tab : public QWidget
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(Tab)
    Q_DISABLE_COPY(Tab)

    Q_PROPERTY(QString currentPath READ currentPath WRITE setCurrentPath NOTIFY currentPathChanged)
    Q_PROPERTY(QString displayName READ displayName NOTIFY displayNameChanged)
public:
    explicit Tab(QWidget *parent = 0);
    ~Tab();

    QString currentPath() const;
    QString displayName() const;

    History *history() const;

    GuiSystem::PerspectiveWidget *perspectiveWidget() const;

public slots:
    void setCurrentPath(const QString &currentPath);

signals:
    void currentPathChanged(const QString & currentPath);
    void displayNameChanged(const QString & currentPath);

private slots:
    void onIndexChanged(int index);

protected:
    void resizeEvent(QResizeEvent *);

protected:
    TabPrivate *d_ptr;
};

} // namespace CorePlugin

#endif // TAB_H
