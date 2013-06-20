#ifndef BROWSERWINDOW_H
#define BROWSERWINDOW_H

#include <Parts/EditorWindow>

class QSettings;
class QUrl;

namespace Parts {
class AbstractEditor;
class EditorView;
class History;
}

namespace Andromeda {

class BrowserWindowPrivate;
class BrowserWindow : public Parts::EditorWindow
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(BrowserWindow)
    Q_DISABLE_COPY(BrowserWindow)
public:
    enum Action { NoAction = -1,
                  Up,
                  NewTab,
                  PrevTab,
                  NextTab,
                  MenuBar,
                  ActionCount
                };

    explicit BrowserWindow(QWidget *parent = 0);
    ~BrowserWindow();

    QAction *action(Action action) const;

    Parts::History *history() const;

    void setEditor(Parts::AbstractEditor *editor);

    static BrowserWindow *currentWindow();
    static QList<BrowserWindow*> windows();

    static QByteArray windowGeometry();
    static void setWindowGeometry(const QByteArray &geometry);

    bool restoreState(const QByteArray &state);
    QByteArray saveState() const;

public slots:
    void back();
    void forward();
    void up();

    void open(const QUrl &url);
    void openNewTab(const QUrl &url);
    void openNewTabs(const QList<QUrl> &urls);
    void openEditor(const QList<QUrl> &urls, const QByteArray &editor);

    void close();

    void newTab();
    static void newWindow();

    void nextTab();
    void previousTab();

    void onUrlChanged(const QUrl &url);

    void onProgressChanged(int progress);
    void onStateChanged(Parts::AbstractDocument::State state);

signals:
    void urlChanged(const QUrl &url);

protected:
    void moveEvent(QMoveEvent *);
    void resizeEvent(QResizeEvent *);

protected slots:
    void onEditorChanged();
    void onTabChanged();

protected:
    BrowserWindowPrivate * d_ptr;
};

} // namespace Andromeda

#endif // BROWSERWINDOW_H
