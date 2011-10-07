#ifndef ABSTRACTEDITOR_H
#define ABSTRACTEDITOR_H

#include "coreplugin_global.h"

#include <QtGui/QIcon>
#include <QtGui/QWidget>

class QSettings;
class QUrl;

namespace CorePlugin {

class AbstractEditorFactory;
class MainWindow;

class COREPLUGIN_EXPORT AbstractEditor : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(AbstractEditor)

public:
    explicit AbstractEditor(QWidget *parent = 0);
    ~AbstractEditor();

    AbstractEditorFactory *factory() const;

    virtual bool open(const QUrl &url) = 0;

    virtual QUrl currentUrl() const = 0;

    virtual int currentIndex() const { return -1; }
    virtual void setCurrentIndex(int index) { Q_UNUSED(index); }

    virtual QIcon icon() const { return QIcon(); }
    virtual QString title() const { return QString(); }
    virtual QString windowTitle() const { return QString(); }

    virtual void restoreSession(QSettings &s);
    virtual void saveSession(QSettings &s);

    MainWindow *mainWindow() const;

    static AbstractEditor *currentEditor();
    template<class T> static T *currentEditor() { return qobject_cast<T*>(currentEditor()); }

signals:
    void currentUrlChanged(const QUrl &);

    void iconChanged(const QIcon &icon);
    void titleChanged(const QString &title);
    void windowTitleChanged(const QString &title);

private:
    void setFactory(AbstractEditorFactory *factory);

private:
    AbstractEditorFactory *m_factory;

    friend class AbstractEditorFactory;
};

} // namespace CorePlugin

#endif // ABSTRACTEDITOR_H
