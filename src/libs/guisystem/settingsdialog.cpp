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

// ----------- Category list view

class CategoryListViewDelegate : public QStyledItemDelegate
{
public:
    CategoryListViewDelegate(QObject *parent) : QStyledItemDelegate(parent) {}
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
        QSize size = QStyledItemDelegate::sizeHint(option, index);
        size.setHeight(qMax(size.height(), 32));
        return size;
    }
};

/**
 * Special version of a QListView that has the width of the first column as
 * minimum size.
 */
class CategoryListView : public QListView
{
public:
    CategoryListView(QWidget *parent = 0) : QListView(parent)
    {
        setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding);
        setItemDelegate(new CategoryListViewDelegate(this));
    }

    virtual QSize sizeHint() const
    {
        int width = sizeHintForColumn(0) + frameWidth() * 2 + 5;
        if (verticalScrollBar()->isVisible())
            width += verticalScrollBar()->width();
        return QSize(width, 100);
    }
};

namespace GuiSystem {

class SettingsDialogPrivate
{
    Q_DECLARE_PUBLIC(SettingsDialog)

public:
    SettingsDialogPrivate(SettingsDialog *qq) : q_ptr(qq) {}

    QLabel *headerLabel;
    QHBoxLayout *headerLayout;
    QStackedLayout *stackedLayout;
    QGridLayout *mainLayout;
    QListView *categoryList;

    QList<QString> categories;
    QList<QTabWidget *> tabWidgets;
    QMap<SettingsPage *, QWidget *> widgets;

    SettingsPageManager *manager;
    QStandardItemModel *model;
    QAction *closeAction;

public:
    void addCategory(const QString &id);
    void addPage(SettingsPage *page);
    void removePage(SettingsPage *page);

    void setupUi();
    void retranslateUi();

private:
    SettingsDialog *q_ptr;
};

} // namespace GuiSystem

using namespace GuiSystem;

static const qint32 settingsDialogMagic = 0x73313267; // "s12g"
static const qint32 settingsDialogVersion = 1;

const int categoryIconSize = 24;

void SettingsDialogPrivate::addCategory(const QString &id)
{
    if (categories.contains(id))
        return;

    SettingsPage *page = manager->pages(id).first();

    QStandardItem *item = new QStandardItem;
    item->setIcon(page->categoryIcon());
    item->setText(page->categoryName());
    item->setData(categories.size());

    model->appendRow(item);
    categories.append(id);

    QTabWidget *widget = new QTabWidget;
    stackedLayout->addWidget(widget);
    tabWidgets.append(widget);
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

void SettingsDialogPrivate::setupUi()
{
    Q_Q(SettingsDialog);

    headerLabel = new QLabel;
    headerLabel->setAlignment(Qt::AlignHCenter);

    // Header label with large font and a bit of spacing (align with group boxes)
    QFont headerLabelFont = headerLabel->font();
    headerLabelFont.setBold(true);
    // Paranoia: Should a font be set in pixels...
    const int pointSize = headerLabelFont.pointSize();
    if (pointSize > 0)
        headerLabelFont.setPointSize(pointSize + 2);
    headerLabel->setFont(headerLabelFont);
    headerLabel->setAlignment(Qt::AlignLeft);

//    const int leftMargin = qApp->style()->pixelMetric(QStyle::PM_LayoutLeftMargin);
//    QSpacerItem *spacer = new QSpacerItem(leftMargin, 0, QSizePolicy::Fixed, QSizePolicy::Ignored);

    headerLayout = new QHBoxLayout;
//    headerLayout->addSpacerItem(spacer);
    headerLayout->addWidget(headerLabel);
    headerLayout->setSpacing(0);
    headerLayout->setMargin(0);

    categoryList = new CategoryListView;
    categoryList->setIconSize(QSize(categoryIconSize, categoryIconSize));
    categoryList->setSelectionMode(QAbstractItemView::SingleSelection);
    categoryList->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    categoryList->setEditTriggers(QAbstractItemView::NoEditTriggers);

    stackedLayout = new QStackedLayout;
    stackedLayout->setMargin(0);

    mainLayout = new QGridLayout;
    mainLayout->addLayout(headerLayout,  0, 1, 1, 1);
    mainLayout->addWidget(categoryList,  0, 0, 2, 1);
    mainLayout->addLayout(stackedLayout, 1, 1, 1, 1);
    mainLayout->setColumnStretch(1, 4);
    q->setLayout(mainLayout);
    q->resize(1024, 576);

    closeAction = new QAction(q);
    closeAction->setShortcut(QKeySequence::Close);
    q->addAction(closeAction);
    QObject::connect(closeAction, SIGNAL(triggered()), q, SLOT(reject()));

    retranslateUi();
}

void SettingsDialogPrivate::retranslateUi()
{
    Q_Q(SettingsDialog);

    q->setWindowTitle(QObject::tr("Preferences"));
}

/*!
    \class SettingsDialog

    \brief SettingsDialog represents application preferences.
*/

/*!
    \brief Creates an empty SettingsDialog with the given \a parent.
*/
SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    d_ptr(new SettingsDialogPrivate(this))
{
    Q_D(SettingsDialog);

    d->manager = 0;
    d->model = new QStandardItemModel(this);
    d->setupUi();

    d->categoryList->setModel(d->model);
    connect(d->categoryList->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this, SLOT(onSelectionChanged(QItemSelection)), Qt::UniqueConnection);

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

    d->categoryList->selectionModel()->select(d->model->index(0, 0), QItemSelectionModel::Select);
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
        d->categoryList->selectionModel()->select(d->model->index(currentPage, 0),
                                                  QItemSelectionModel::Select | QItemSelectionModel::Clear);
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
        d->headerLabel->setText(index.data(Qt::DisplayRole).toString());
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
