#ifndef BROWSERWINDOW_P_H
#define BROWSERWINDOW_P_H

#include "browserwindow.h"

#include "tabbar.h"

#include <QtCore/QEvent>
#include <QtGui/QAbstractButton>
#include <QtGui/QApplication>
#include <QtGui/QMouseEvent>
#include <QtGui/QPainter>
#include <QtGui/QStyleOptionTabBarBaseV2>
#include <QtGui/QTabWidget>
#include <QtGui/QToolBar>
#include <widgets/addressbar.h>
#include <guisystem/stackededitor.h>

class MyTabWidget : public QTabWidget
{
    Q_OBJECT
public:
    MyTabWidget(QWidget *parent = 0) : QTabWidget(parent)
    {
        TabBar *tabBar = new TabBar;
        tabBar->setSwitchTabsOnDrag(true);
        tabBar->installEventFilter(this);
        setTabBar(tabBar);
    }

signals:
    void tabBarDoubleClicked();

protected:
    bool eventFilter(QObject *o, QEvent *e)
    {
        if (o != tabBar())
            return false;

        if (e->type() == QEvent::MouseButtonDblClick) {
            QMouseEvent *me = static_cast<QMouseEvent *>(e);

            if (tabBar()->tabAt(me->pos()) == -1)
                emit tabBarDoubleClicked();

            return true;
        }
        return false;
    }
};

class TabBarButton : public QAbstractButton
{
public:
    TabBarButton() :
        QAbstractButton(),
        hovered(false),
        pressed(false)
    {
    }

    void paintEvent(QPaintEvent *e)
    {
        QPainter p(this);

        QStyleOptionTabBarBaseV2 opt;
        opt.init(this);
        // hardcoded document Mode
        opt.documentMode = true;

        int overlap = style()->pixelMetric(QStyle::PM_TabBarBaseOverlap, &opt, this);
        QRect rect;
        // hardcoded north position
        rect.setRect(0, size().height() - overlap, size().width(), overlap);
        opt.rect = rect;

        qApp->style()->drawPrimitive(QStyle::PE_FrameTabBarBase, &opt, &p, this);

        int w = iconSize().width();
        int h = iconSize().height();
        QIcon::Mode mode = pressed ? QIcon::Selected : QIcon::Normal;

        icon().paint(&p, (width() - w)/2, (height() - h)/2, w, h, Qt::AlignCenter, mode);
    }

    QSize sizeHint() const { return iconSize(); }
    void enterEvent(QEvent *) { hovered = true; update(); }
    void leaveEvent(QEvent *) { hovered = false; update(); }
    void mousePressEvent(QMouseEvent *e) { pressed = true; update(); QAbstractButton::mousePressEvent(e); }
    void mouseReleaseEvent(QMouseEvent *e) { pressed = false; update(); QAbstractButton::mouseReleaseEvent(e); }

private:
    bool hovered;
    bool pressed;
};

namespace CorePlugin {

class BrowserWindowPrivate : public QObject
{
    Q_OBJECT
    Q_DECLARE_PUBLIC(BrowserWindow)
    BrowserWindow *q_ptr;
public:
    BrowserWindowPrivate(BrowserWindow *qq) : q_ptr(qq) {}

    AddressBar *lineEdit;
    QToolBar *toolBar;
    QTabWidget *tabWidget;
    TabBarButton *newTabButton;

    QAction *backAction;
    QAction *forwardAction;
    QAction *upAction;

    QAction *nextTabAction;
    QAction *prevTabAction;

    QAction *newTabAction;
    QAction *closeTabAction;

    QAction *saveAction;
    QAction *saveAsAction;

    QAction *undoAction;
    QAction *redoAction;

    QAction *cutAction;
    QAction *copyAction;
    QAction *pasteAction;
    QAction *selectAllAction;

    QSettings *settings;

    GuiSystem::StackedEditor *addTab(int *index = 0);
    void createAction(QAction *&action, const QString &text, const QByteArray &id, QWidget *w, const char *slot);
    void setupActions();
    void setupToolBar();
    void setupAlternateToolBar();
    void setupUi();
    void updateUi(GuiSystem::StackedEditor *tab);

public slots:
    void onUrlChanged(const QUrl &);
    void onCurrentChanged(int);
    void onChanged();

    void onCapabilitiesChanged(GuiSystem::AbstractEditor::Capabilities capabilities);
};

} // namespace CorePlugin

#endif // BROWSERWINDOW_P_H
