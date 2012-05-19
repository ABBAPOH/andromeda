#ifndef MAINWINDOWFACTORY_H
#define MAINWINDOWFACTORY_H

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtCore/QUrl>

namespace GuiSystem {

class MainWindow;

class MainWindowFactory : public QObject
{
    Q_OBJECT
public:
    enum Capability {
        Open = 0x1, // open in current tab
        OpenNewEditor = 0x2, // open in new tabs
        OpenNewWindow = 0x4, // open new window with urls in it
        OpenNewWindows = 0x8 // open new windows with single url each
    };
    Q_DECLARE_FLAGS(Capabilities, Capability)

    explicit MainWindowFactory(QObject *parent = 0);

    static MainWindowFactory *defaultFactory();
    static void setDefaultfactory(MainWindowFactory *factory);

    MainWindow *activeWindow() const;

    virtual Capabilities capabilities() const;
    virtual void open(Capability cap, QList<QUrl> urls);
    virtual void openEditor(const QString &id);

    virtual MainWindow *create();

protected:
    bool eventFilter(QObject *, QEvent *);

protected:
    QPointer<MainWindow> m_activeWindow;
};

} // namespace GuiSystem

#endif // MAINWINDOWFACTORY_H
