#ifndef MAINWINDOWFACTORY_H
#define MAINWINDOWFACTORY_H

#include "guisystem_global.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtCore/QUrl>

namespace GuiSystem {

class MainWindow;

class GUISYSTEM_EXPORT MainWindowFactory : public QObject
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

    explicit MainWindowFactory(QObject *parent = 0);

    static MainWindowFactory *defaultFactory();
    static void setDefaultfactory(MainWindowFactory *factory);

    MainWindow *activeWindow() const;

    virtual OpenFlags openFlags() const;
    virtual void openFlag(OpenFlag cap, QList<QUrl> urls);

    void open(const QUrl &url);
    void open(const QList<QUrl> &urls);

    void openNewEditor(const QUrl &url);
    void openNewEditor(const QList<QUrl> &urls);

    void openNewWindow(const QUrl &url);
    void openNewWindow(const QList<QUrl> &urls);

    void openNewWindows(const QList<QUrl> &urls);

    void openEditor(const QString &id);

    virtual MainWindow *create();

protected:
    bool eventFilter(QObject *, QEvent *);

protected:
    QPointer<MainWindow> m_activeWindow;
};

} // namespace GuiSystem

#endif // MAINWINDOWFACTORY_H
