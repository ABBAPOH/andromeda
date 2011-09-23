#ifndef ABSTRACTEDITOR_H
#define ABSTRACTEDITOR_H

#include <QtGui/QIcon>
#include <QtGui/QWidget>

class QSettings;

namespace CorePlugin {

class AbstractEditorFactory;

class AbstractEditor : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(AbstractEditor)

public:
    explicit AbstractEditor(QWidget *parent = 0);
    ~AbstractEditor();

    AbstractEditorFactory *factory() const;

    virtual bool open(const QString &path) = 0;

    virtual QString currentPath() const = 0;

    virtual int currentIndex() const { return -1; }
    virtual void setCurrentIndex(int index) { Q_UNUSED(index); }

    virtual QIcon icon() const { return QIcon(); }
    virtual QString title() const { return QString(); }
    virtual QString windowTitle() const { return QString(); }

    void restoreSession(const QSettings &s);
    void saveSession(QSettings &s);

//    MainWindow *mainWindow() const;

signals:
    void currentPathChanged(const QString &);

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
