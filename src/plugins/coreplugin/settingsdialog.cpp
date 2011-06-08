#include "settingsdialog.h"
#include "settingsdialog_p.h"

#include <QtGui/QApplication>
#include <QtGui/QLineEdit>
#include <QtGui/QScrollBar>
#include <QtGui/QStyledItemDelegate>
#include <QtGui/QTabWidget>

using namespace CorePlugin;

const int categoryIconSize = 24;

// Helpers to sort by category. id
bool optionsPageLessThan(const IOptionsPage *p1, const IOptionsPage *p2)
{
    if (const int cc = p1->category().compare(p2->category()))
        return cc < 0;
    return p1->id().compare(p2->id()) < 0;
}

CategoryModel::CategoryModel(QObject *parent)
    : QAbstractListModel(parent)
{
    QPixmap empty(categoryIconSize, categoryIconSize);
    empty.fill(Qt::transparent);
    m_emptyIcon = QIcon(empty);
}

CategoryModel::~CategoryModel()
{
    qDeleteAll(m_categories);
}

int CategoryModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_categories.size();
}

QVariant CategoryModel::data(const QModelIndex &index, int role) const
{
    switch (role) {
    case Qt::DisplayRole:
        return m_categories.at(index.row())->displayName();
    case Qt::DecorationRole: {
        QIcon icon = m_categories.at(index.row())->icon();
            if (icon.isNull())
                icon = m_emptyIcon;
            return icon;
        }
    }

    return QVariant();
}

void CategoryModel::addPage(IOptionsPage *page)
{
    const QString &categoryId = page->category();
    Category *category = findCategoryById(categoryId);
    if (!category) {
        category = new Category;
        category->id = categoryId;
        category->index = -1;
        beginInsertRows(QModelIndex(), m_categories.size(), m_categories.size());
        m_categories.append(category);
        qStableSort(category->pages.begin(), category->pages.end(), optionsPageLessThan);
        endInsertRows();
    }
    category->pages.append(page);
}

void CategoryModel::removePage(IOptionsPage *page)
{
    const QString &categoryId = page->category();
    Category *category = findCategoryById(categoryId);
    if (category) {
        emit pageRemoved(page);

        if (category->pages.isEmpty()) {
            int index = m_categories.indexOf(category);
            beginRemoveRows(QModelIndex(), index, index);
            m_categories.takeAt(index);
            delete category;
            endRemoveRows();
        }
    }
}

Category *CategoryModel::findCategoryById(const QString &id)
{
    for (int i = 0; i < m_categories.size(); ++i) {
        Category *category = m_categories.at(i);
        if (category->id == id)
            return category;
    }

    return 0;
}

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

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    m_model(0),
    m_stackedLayout(new QStackedLayout),
    m_categoryList(new CategoryListView),
    m_headerLabel(new QLabel)
{
    setupUi();
}

SettingsDialog::~SettingsDialog()
{
}

CategoryModel * SettingsDialog::model()
{
    return m_model;
}

void SettingsDialog::setModel(CategoryModel *model)
{
    m_model = model;
    m_categoryList->setModel(m_model);
    connect(m_categoryList->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            this, SLOT(currentChanged(QModelIndex)), Qt::UniqueConnection);
    connect(m_model, SIGNAL(pageAdded(IOptionsPage*)), SLOT(onPageRemoved(IOptionsPage*)));
    connect(m_model, SIGNAL(pageRemoved(IOptionsPage*)), SLOT(onPageRemoved(IOptionsPage*)));
}

void SettingsDialog::currentChanged(const QModelIndex &current)
{
    if (current.isValid())
        showCategory(current.row());
}

void SettingsDialog::onPageAdded(IOptionsPage *page)
{
    Category *category = m_model->findCategoryById(page->category());
    QTabWidget *widget = m_tabWidgets.value(category);
    int index = category->pages.indexOf(page);
    widget->insertTab(index, page->createPage(widget), page->displayName());
}

void SettingsDialog::onPageRemoved(IOptionsPage *page)
{
    QWidget *widget = m_widgets.take(page);
    delete widget;
}

void SettingsDialog::setupUi()
{
    // Header label with large font and a bit of spacing (align with group boxes)
    QFont headerLabelFont = m_headerLabel->font();
    headerLabelFont.setBold(true);
    // Paranoia: Should a font be set in pixels...
    const int pointSize = headerLabelFont.pointSize();
    if (pointSize > 0)
        headerLabelFont.setPointSize(pointSize + 2);
    m_headerLabel->setFont(headerLabelFont);

    QHBoxLayout *headerLayout = new QHBoxLayout;
    const int leftMargin = qApp->style()->pixelMetric(QStyle::PM_LayoutLeftMargin);
    QSpacerItem *spacer = new QSpacerItem(leftMargin, 0, QSizePolicy::Fixed, QSizePolicy::Ignored);
    headerLayout->addSpacerItem(spacer);
    m_headerLabel->setAlignment(Qt::AlignHCenter);
    headerLayout->addWidget(m_headerLabel);

    m_stackedLayout->setMargin(0);

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addLayout(headerLayout,     0, 1, 1, 1);
    mainLayout->addWidget(m_categoryList,   0, 0, 2, 1);
    mainLayout->addLayout(m_stackedLayout,  1, 1, 1, 1);
    mainLayout->setColumnStretch(1, 4);
    setLayout(mainLayout);
    setMinimumSize(1024, 576);
}

void SettingsDialog::showCategory(int index)
{
    Category *category = m_model->categories().at(index);
    ensureCategoryWidget(category);
    // Update current category and page
    m_currentCategory = category->id;
    const int currentTabIndex = m_tabWidgets.value(category)->currentIndex();
    if (currentTabIndex != -1) {
        IOptionsPage *page = category->pages.at(currentTabIndex);
        m_currentPage = page->id();
    }

    m_stackedLayout->setCurrentIndex(category->index);
    m_headerLabel->setText(category->displayName());
}

void SettingsDialog::ensureCategoryWidget(Category *category)
{
    if (m_tabWidgets.value(category) != 0)
        return;

    QTabWidget *tabWidget = new QTabWidget;
    for (int j = 0; j < category->pages.size(); ++j) {
        IOptionsPage *page = category->pages.at(j);
        QWidget *widget = page->createPage(0);
        m_widgets.insert(page, widget);
        tabWidget->addTab(widget, page->displayName());
    }

    connect(tabWidget, SIGNAL(currentChanged(int)),
            this, SLOT(currentTabChanged(int)));

    m_tabWidgets.insert(category, tabWidget);
    category->index = m_stackedLayout->addWidget(tabWidget);
}
