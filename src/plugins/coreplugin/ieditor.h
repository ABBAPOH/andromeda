#ifndef IMAINVIEW_H
#define IMAINVIEW_H

#include <QtCore/QString>
#include <iview.h>

#include "historyitem.h"

namespace CorePlugin {

class MainWindow;

class COREPLUGIN_EXPORT IEditor : public GuiSystem::IView
{
    Q_OBJECT
    Q_DISABLE_COPY(IEditor)

public:
    explicit IEditor(QObject *parent = 0);

    virtual bool open(const QString &path) = 0;

    virtual QString currentPath() const = 0;

    virtual int currentIndex() const { return -1; }
    virtual void setCurrentIndex(int index) { Q_UNUSED(index); }

    virtual QIcon icon() const { return QIcon(); }
    virtual QString title() const { return QString(); }
    virtual QString windowTitle() const { return QString(); }

    void restoreSession(const QSettings &s);
    void saveSession(QSettings &s);

    MainWindow *mainWindow() const;

signals:
    void pathChanged(const QString &);
    void currentItemChanged();
    void changed();
};

} // namespace CorePlugin

#endif // IMAINVIEW_H
