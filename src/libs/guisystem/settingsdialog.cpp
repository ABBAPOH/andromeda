#include "settingsdialog.h"

#include "settingspage.h"
#include "settingspagemanager.h"

#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QLineEdit>
#include <QtGui/QStandardItemModel>
#include <QtGui/QScrollBar>
#include <QtGui/QStyledItemDelegate>
#include <QtGui/QTabWidget>

#include <widgets/modeltoolbar.h>

namespace GuiSystem {

class SettingsDialogPrivate
{
    Q_DECLARE_PUBLIC(SettingsDialog)

public:
    SettingsDialogPrivate(SettingsDialog *qq) : q_ptr(qq) {}

    QStackedLayout *stackedLayout;
    QGridLayout *mainLayout;
    ModelToolBar *toolbar;

    QList<QString> categories;
    QList<QTabWidget *> tabWidgets;
    QMap<SettingsPage *, QWidget *> widgets;

    SettingsPageManager *manager;
    QStandardItemModel *model;
    QAction *closeAction;
    bool ignoreDataChanged;

public:
    void addCategory(const QString &id);
    void addPage(SettingsPage *page);
    void removePage(SettingsPage *page);
    void selectRow(int row);

    void setupUi();
    void retranslateUi();

private:
    SettingsDialog *q_ptr;
};

} // namespace GuiSystem

using namespace GuiSystem;

static const qint32 settingsDialogMagic = 0x73313267; // "s12g"
static const qint32 settingsDialogVersion = 1;

const int categoryIconSize = 32;

void SettingsDialogPrivate::addCategory(const QString &id)
{
    if (categories.contains(id))
        return;

    SettingsPage *page = manager->pages(id).first();

    QStandardItem *item = new QStandardItem;
    item->setIcon(page->categoryIcon());
    item->setText(page->categoryName());
    item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
    item->setData(categories.size());

    model->appendRow(item);
    categories.append(id);

    QTabWidget *tabWidget = new QTabWidget;
    tabWidget->setFocusPolicy(Qt::NoFocus);
    stackedLayout->addWidget(tabWidget);
    tabWidgets.append(tabWidget);
}

void SettingsDialogPrivate::addPage(SettingsPage *page)
{
    int index = categories.indexOf(page->category());

    QTabWidget *tabWidget = static_cast<QTabWidget *>(stackedLayout->widget(index));
    QWidget *widget = page->createPage(tabWidget);
    widgets.insert(page, widget);
    tabWidget->addTab(widget, page->name());
}

void SettingsDialogPrivate::removePage(SettingsPage *page)
{
    QWidget *widget = widgets.take(page);
    if (widget) {
        delete widget;
    }
}

void SettingsDialogPrivate::selectRow(int row)
{
    model->setData(model->index(row, 0), Qt::Checked, Qt::CheckStateRole);
}

void SettingsDialogPrivate::setupUi()
{
    Q_Q(SettingsDialog);

    toolbar = new ModelToolBar;
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

void SettingsDialogPrivate::retranslateUi()
{
    Q_Q(SettingsDialog);

    q->setWindowTitle(SettingsDialog::tr("Preferences"));
}

/*!
    \class SettingsDialog

    \brief SettingsDialog represents application preferences.
*/

/*!
    \brief Creates an empty SettingsDialog with the given \a parent.
*/
SettingsDialog::SettingsDialog(QWidget *parent) :
    QMainWindow(parent),
    d_ptr(new SettingsDialogPrivate(this))
{
    Q_D(SettingsDialog);

    d->manager = 0;
    d->model = new QStandardItemModel(this);
    d->ignoreDataChanged = false;
    d->setupUi();

    d->toolbar->setModel(d->model);
    connect(d->model, SIGNAL(dataChanged(QModelIndex,QModelIndex)), SLOT(onDataChanged(QModelIndex,QModelIndex)));

    setObjectName(QLatin1String("SettingsDialog"));
}

/*!
    \brief Destroys SettingsDialog.
*/
SettingsDialog::~SettingsDialog()
{
    delete d_ptr;
}

/*!

    \brief Returns SettingsManager assigned to this dialog.
*/
SettingsPageManager *SettingsDialog::settingsPageManager() const
{
    return d_func()->manager;
}

/*!
    \brief Assigned \a manager to this dialog fill it with settings pages.
*/
void SettingsDialog::setSettingsPageManager(SettingsPageManager *manager)
{
    Q_D(SettingsDialog);

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

    d->selectRow(0);
}

QByteArray SettingsDialog::saveState() const
{
    Q_D(const SettingsDialog);

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

bool SettingsDialog::restoreState(const QByteArray &arr)
{
    Q_D(SettingsDialog);

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
        d->selectRow(currentPage);
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
void SettingsDialog::onSelectionChanged(const QItemSelection &newSelection)
{
    Q_D(SettingsDialog);

    if (!newSelection.isEmpty()) {
        QModelIndex index = newSelection.indexes().first();
        d->stackedLayout->setCurrentIndex(index.data(Qt::UserRole + 1).toInt());
    }
}

/*!
    \internal
*/
void SettingsDialog::onPageAdded(SettingsPage *page)
{
    Q_D(SettingsDialog);

    QString category = page->category();
    d->addCategory(category);
    d->addPage(page);
}

/*!
    \internal
*/
void SettingsDialog::onPageRemoved(SettingsPage *page)
{
    d_func()->removePage(page);
}

void SettingsDialog::onDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    Q_D(SettingsDialog);

    if (d->ignoreDataChanged)
        return;

    for (int row = topLeft.row(); row <= bottomRight.row(); row++) {
        QModelIndex index = d->model->index(row, 0);
        if (index.data(Qt::CheckStateRole).toInt() == Qt::Checked) {
            int oldIndex = d->stackedLayout->currentIndex();
            if (row != oldIndex) { // special case when selecting first time
                d->ignoreDataChanged = true;
                d->model->setData(d->model->index(oldIndex, 0), Qt::Unchecked, Qt::CheckStateRole);
                d->ignoreDataChanged = false;
            }
            d->stackedLayout->setCurrentIndex(row);
            return;
        }
    }
}
