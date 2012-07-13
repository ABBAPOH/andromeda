#include "settingswindow.h"

#include "settingspage.h"
#include "settingspagemanager.h"

#include <QtGui/QAction>
#include <QtGui/QTabWidget>
#include <QtGui/QToolBar>

namespace GuiSystem {

class SettingsWindowPrivate
{
    Q_DECLARE_PUBLIC(SettingsWindow)

public:
    SettingsWindowPrivate(SettingsWindow *qq) : q_ptr(qq) {}

    QStackedLayout *stackedLayout;
    QGridLayout *mainLayout;
    QToolBar *toolbar;

    QList<QString> categories;
    QList<QTabWidget *> tabWidgets;
    QMap<SettingsPage *, QWidget *> widgets;

    SettingsPageManager *manager;
    QAction *closeAction;
    QActionGroup *actionGroup;

public:
    void addCategory(const QString &id);
    void addPage(SettingsPage *page);
    void removePage(SettingsPage *page);
    void selectPage(int row);

    void setupUi();
    void retranslateUi();

private:
    SettingsWindow *q_ptr;
};

} // namespace GuiSystem

using namespace GuiSystem;

static const qint32 settingsDialogMagic = 0x73313267; // "s12g"
static const qint32 settingsDialogVersion = 1;

const int categoryIconSize = 32;

void SettingsWindowPrivate::addCategory(const QString &id)
{
    Q_Q(SettingsWindow);

    if (categories.contains(id))
        return;

    SettingsPage *page = manager->pages(id).first();

    QAction *action = new QAction(toolbar);
    action->setCheckable(true);
    action->setIcon(page->categoryIcon());
    action->setText(page->categoryName());
    action->setData(categories.size());
    actionGroup->addAction(action);
    toolbar->addAction(action);
    q->connect(action, SIGNAL(triggered(bool)), q, SLOT(onActionTriggered(bool)));

    categories.append(id);

    QTabWidget *tabWidget = new QTabWidget;
    tabWidget->setFocusPolicy(Qt::NoFocus);
    stackedLayout->addWidget(tabWidget);
    tabWidgets.append(tabWidget);
}

void SettingsWindowPrivate::addPage(SettingsPage *page)
{
    int index = categories.indexOf(page->category());

    QTabWidget *tabWidget = static_cast<QTabWidget *>(stackedLayout->widget(index));
    QWidget *widget = page->createPage(tabWidget);
    widgets.insert(page, widget);
    tabWidget->addTab(widget, page->name());
}

void SettingsWindowPrivate::removePage(SettingsPage *page)
{
    QWidget *widget = widgets.take(page);
    if (widget) {
        delete widget;
    }
}

void SettingsWindowPrivate::selectPage(int index)
{
    toolbar->actions().at(index)->trigger();
}

void SettingsWindowPrivate::setupUi()
{
    Q_Q(SettingsWindow);

    actionGroup = new QActionGroup(q);
    actionGroup->setExclusive(true);

    toolbar = new QToolBar;
    toolbar->setIconSize(QSize(categoryIconSize, categoryIconSize));
    toolbar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    q->addToolBar(toolbar);
    q->setUnifiedTitleAndToolBarOnMac(true);

    q->setCentralWidget(new QWidget);
    stackedLayout = new QStackedLayout();

    mainLayout = new QGridLayout(q->centralWidget());
    mainLayout->addLayout(stackedLayout, 0, 0);
    q->resize(1024, 576);

    closeAction = new QAction(q);
    closeAction->setShortcut(QKeySequence::Close);
    q->addAction(closeAction);
    QObject::connect(closeAction, SIGNAL(triggered()), q, SLOT(close()));

    retranslateUi();
}

void SettingsWindowPrivate::retranslateUi()
{
    Q_Q(SettingsWindow);

    q->setWindowTitle(SettingsWindow::tr("Preferences"));
}

/*!
    \class SettingsDialog

    \brief SettingsDialog represents application preferences.
*/

/*!
    \brief Creates an empty SettingsDialog with the given \a parent.
*/
SettingsWindow::SettingsWindow(QWidget *parent) :
    QMainWindow(parent),
    d_ptr(new SettingsWindowPrivate(this))
{
    Q_D(SettingsWindow);

    d->manager = 0;
    d->setupUi();

    setObjectName(QLatin1String("SettingsDialog"));
}

/*!
    \brief Destroys SettingsDialog.
*/
SettingsWindow::~SettingsWindow()
{
    delete d_ptr;
}

/*!

    \brief Returns SettingsManager assigned to this dialog.
*/
SettingsPageManager *SettingsWindow::settingsPageManager() const
{
    return d_func()->manager;
}

/*!
    \brief Assigned \a manager to this dialog fill it with settings pages.
*/
void SettingsWindow::setSettingsPageManager(SettingsPageManager *manager)
{
    Q_D(SettingsWindow);

    if (d->manager) {
        // clear;
        disconnect(d->manager, 0, this, 0);
    }

    d->manager = manager;

    foreach (const QString &category, manager->categories()) {
        d->addCategory(category);
        foreach (SettingsPage *page, d->manager->pages(category)) {
            d->addPage(page);
        }
    }
    connect(d->manager, SIGNAL(pageAdded(SettingsPage*)), SLOT(onPageAdded(SettingsPage*)));
    connect(d->manager, SIGNAL(pageRemoved(SettingsPage*)), SLOT(onPageRemoved(SettingsPage*)));

    d->selectPage(0);
}

QByteArray SettingsWindow::saveState() const
{
    Q_D(const SettingsWindow);

    qint32 currentPage = d->stackedLayout->currentIndex();
    qint32 currentTab = d->tabWidgets[currentPage]->currentIndex();

    QByteArray state;
    QDataStream s(&state, QIODevice::WriteOnly);

    s << settingsDialogMagic;
    s << settingsDialogVersion;
    s << currentPage;
    s << currentTab;
    s << saveGeometry();

    return state;
}

bool SettingsWindow::restoreState(const QByteArray &arr)
{
    Q_D(SettingsWindow);

    QByteArray state = arr;

    QDataStream s(&state, QIODevice::ReadOnly);

    qint32 magic, version, currentPage, currentTab;
    QByteArray geometry;

    s >> magic;
    if (magic != settingsDialogMagic)
        return false;

    s >> version;
    if (version != settingsDialogVersion)
        return false;

    s >> currentPage;
    if (0 <= currentPage && currentPage < d->stackedLayout->count()) {
        d->selectPage(currentPage);
    } else {
        return false;
    }

    s >> currentTab;
    if (0 <= currentTab && currentTab < d->tabWidgets[currentPage]->count())
        d->tabWidgets[currentPage]->setCurrentIndex(currentTab);
    else
        return false;

    s >> geometry;

    return restoreGeometry(geometry);
}

/*!
    \internal
*/
void SettingsWindow::onSelectionChanged(const QItemSelection &newSelection)
{
    Q_D(SettingsWindow);

    if (!newSelection.isEmpty()) {
        QModelIndex index = newSelection.indexes().first();
        d->stackedLayout->setCurrentIndex(index.data(Qt::UserRole + 1).toInt());
    }
}

/*!
    \internal
*/
void SettingsWindow::onPageAdded(SettingsPage *page)
{
    Q_D(SettingsWindow);

    QString category = page->category();
    d->addCategory(category);
    d->addPage(page);
}

/*!
    \internal
*/
void SettingsWindow::onPageRemoved(SettingsPage *page)
{
    d_func()->removePage(page);
}

void SettingsWindow::onActionTriggered(bool toggled)
{
    Q_D(SettingsWindow);
    QAction *action = qobject_cast<QAction*>(sender());
    int index = d->toolbar->actions().indexOf(action);
    d->stackedLayout->setCurrentIndex(index);
}
