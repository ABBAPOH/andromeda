#ifndef BROWSERWINDOW_H
#define BROWSERWINDOW_H

#include "coreplugin_global.h"

#include <QtGui/QMainWindow>

class QSettings;
class QUrl;

namespace GuiSystem {
class AbstractEditor;
class StackedContainer;
}

namespace CorePlugin {

class BrowserWindowPrivate;
class COREPLUGIN_EXPORT BrowserWindow : public QMainWindow
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(BrowserWindow)

public:
    explicit BrowserWindow(QWidget *parent = 0);
    ~BrowserWindow();

    int currentIndex() const;
    GuiSystem::StackedContainer *currentTab() const;
    int count() const;

    void restoreSession(QSettings &s);
    void saveSession(QSettings &s);

    GuiSystem::AbstractEditor *currentEditor() const;

    static BrowserWindow *currentWindow();
    static QList<BrowserWindow*> windows();
    static BrowserWindow *createWindow();

public slots:
    void back();
    void forward();
    void up();

    void open(const QUrl &url);
    void openEditor(const QString &id);
    void openNewTab(const QUrl &url);
    void openNewTab(const QList<QUrl> &urls);
    static void openNewWindow(const QUrl &url);
    static void openNewWindow(const QList<QUrl> &urls);

    void newTab();
    static void newWindow();
    void closeTab(int index = -1);

    void save();
    void saveAs();

    void refresh();
    void cancel();

    void nextTab();
    void prevTab();

protected:
    void moveEvent(QMoveEvent *);
    void resizeEvent(QResizeEvent *);

protected:
    BrowserWindowPrivate * d_ptr;
};

} // namespace CorePlugin

#endif // BROWSERWINDOW_H
