#ifndef MAINWINDOWFACTORY_H
#define MAINWINDOWFACTORY_H

#include "../guisystem_global.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtCore/QUrl>

namespace GuiSystem {

class EditorWindow;

class GUISYSTEM_EXPORT EditorWindowFactory : public QObject
{
    Q_OBJECT
public:
    enum OpenFlag {
        Open = 0x1, // open in current tab
        OpenNewEditor = 0x2, // open in new tabs
        OpenNewWindow = 0x4, // open new window with urls in it
        OpenNewWindows = 0x8 // open new windows with single url each
    };
    Q_DECLARE_FLAGS(OpenFlags, OpenFlag)

    explicit EditorWindowFactory(QObject *parent = 0);

    static EditorWindowFactory *defaultFactory();
    static void setDefaultfactory(EditorWindowFactory *factory);

    EditorWindow *activeWindow() const;

    virtual OpenFlags openFlags() const;

    void open(const QUrl &url);
    virtual void open(const QList<QUrl> &urls);
    virtual void openEditor(const QList<QUrl> &urls, const QByteArray &editor);

    void openNewEditor(const QUrl &url);
    virtual void openNewEditor(const QList<QUrl> &urls);

    void openNewWindow(const QUrl &url);
    virtual void openNewWindow(const QList<QUrl> &urls);

    void openNewWindows(const QList<QUrl> &urls);

    void openEditor(const QString &id);

    virtual EditorWindow *create();

protected:
    bool eventFilter(QObject *, QEvent *);

protected:
    QPointer<EditorWindow> m_activeWindow;
};

} // namespace GuiSystem

#endif // MAINWINDOWFACTORY_H
