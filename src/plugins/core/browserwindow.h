#ifndef BROWSERWINDOW_H
#define BROWSERWINDOW_H

#include "core_global.h"

#include <guisystem/editorwindow.h>

class QSettings;
class QUrl;

namespace GuiSystem {
class AbstractEditor;
class StackedContainer;
}

namespace Core {

class BrowserWindowPrivate;
class CORE_EXPORT BrowserWindow : public GuiSystem::EditorWindow
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(BrowserWindow)

public:
    explicit BrowserWindow(QWidget *parent = 0);
    ~BrowserWindow();

    static BrowserWindow *currentWindow();
    static QList<BrowserWindow*> windows();

    static QByteArray windowGeometry();
    static void setWindowGeometry(const QByteArray &geometry);

public slots:
    void up();

    void open(const QUrl &url);
    void openNewTab(const QUrl &url);
    void openNewTabs(const QList<QUrl> &urls);

    void close();

    void newTab();
    static void newWindow();

    void nextTab();
    void previousTab();

    void onUrlChanged(const QUrl &url);

    void startLoad();
    void setLoadProgress(int progress);
    void finishLoad(bool ok);

protected:
    void moveEvent(QMoveEvent *);
    void resizeEvent(QResizeEvent *);

protected:
    BrowserWindowPrivate * d_ptr;
};

} // namespace Core

#endif // BROWSERWINDOW_H
