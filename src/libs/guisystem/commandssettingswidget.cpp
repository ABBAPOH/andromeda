#include "commandssettingswidget.h"
#include "ui_commandssettingswidget.h"

#include "command.h"
#include "commandsmodel.h"

#include <QtCore/QEvent>
#include <QtGui/QFileDialog>
#include <QtGui/QKeyEvent>
#include <QtGui/QMessageBox>
#include <QtGui/QSortFilterProxyModel>
#include <QtGui/QStyledItemDelegate>

#include <widgets/shortcutedit.h>

using namespace GuiSystem;

class FolderProxyModel : public QSortFilterProxyModel
{
public:
    explicit FolderProxyModel(QObject *parent = 0);

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;
};

FolderProxyModel::FolderProxyModel(QObject *parent) :
    QSortFilterProxyModel(parent)
{
}

// Cut off my hands :)
bool FolderProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    QAbstractItemModel *source = sourceModel();
    if (!source)
        return QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);

    QModelIndex index = source->index(source_row, 0, source_parent);
    if (source->hasChildren(index)) {
        if (QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent))
            return true;

        bool ok = false;
        for (int i = 0; !ok && i < source->rowCount(index); i++) {
            ok |= filterAcceptsRow(i, index);
        }
        return ok;
    } else {
        bool ok = false;
        do {
            ok |= QSortFilterProxyModel::filterAcceptsRow(index.row(), index.parent());
            index = index.parent();
        } while (!ok && index.isValid());
        return ok;
    }
}

class ShortcutDelegate : public QStyledItemDelegate
{
public:
    explicit ShortcutDelegate(QObject *parent = 0);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    bool eventFilter(QObject *object, QEvent *event);
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
};

ShortcutDelegate::ShortcutDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{
}

QWidget * ShortcutDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return new ShortcutEdit(parent);
}

bool ShortcutDelegate::eventFilter(QObject *object, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        return false;
    }
    return QStyledItemDelegate::eventFilter(object, event);
}

void ShortcutDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    ShortcutEdit *edit = qobject_cast<ShortcutEdit *>(editor);
    Q_ASSERT(edit);
    edit->setKeySequence(index.data().toString());
}

/*!
    \class GuiSystem::CommandsSettingsWidget

    \brief CommandsSettingsWidget is a widget for displaying and editing available Commands.

    This widget displays CommandsModel and allows user to change/reset and
    import/export shortcuts. Also this widget provides filter line edit to
    simplify searching for a shortcut.

    \image html commandsettingswidget.png
*/

/*!
    Creates CommandsSettingsWidget with the given \a parent.
*/
CommandsSettingsWidget::CommandsSettingsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CommandsSettingsWidget),
    m_model(new CommandsModel(this)),
    m_proxy(new FolderProxyModel(this))
{
    ui->setupUi(this);

    m_proxy->setSourceModel(m_model);
    m_proxy->setFilterCaseSensitivity(Qt::CaseInsensitive);
    m_proxy->setFilterKeyColumn(-1);

    ui->view->setModel(m_proxy);
    ui->view->sortByColumn(0, Qt::AscendingOrder);
    ui->view->expandAll();
    ui->view->setColumnWidth(0, 300);
    ui->view->setItemDelegateForColumn(1, new ShortcutDelegate(ui->view));

    connect(ui->view->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            SLOT(onSelectionChanged(QItemSelection)));
    connect(ui->filterEdit, SIGNAL(textChanged(QString)), SLOT(onTextChanged(QString)));

    connect(ui->resetButton, SIGNAL(clicked()), SLOT(reset()));
    connect(ui->resetAllButton, SIGNAL(clicked()), SLOT(resetAll()));

    connect(ui->importButton, SIGNAL(clicked()), SLOT(importShortcuts()));
    connect(ui->exportButton, SIGNAL(clicked()), SLOT(exportShortcuts()));
}

/*!
    Destroys CommandsSettingsWidget.
*/
CommandsSettingsWidget::~CommandsSettingsWidget()
{
    delete ui;
}

void CommandsSettingsWidget::onTextChanged(const QString &text)
{
    m_proxy->setFilterFixedString(text);
    ui->view->sortByColumn(0, Qt::AscendingOrder);
    ui->view->expandAll();
}

void CommandsSettingsWidget::onSelectionChanged(const QItemSelection &selection)
{
    if (selection.indexes().isEmpty()) {
        ui->resetButton->setEnabled(false);
    } else {
        QModelIndex index = selection.indexes().first();
        ui->resetButton->setEnabled(index.parent().isValid() && m_model->isModified(m_proxy->mapToSource(index)));
    }
}

void CommandsSettingsWidget::reset()
{
    QModelIndexList rows = ui->view->selectionModel()->selectedRows();
    if (rows.isEmpty())
        return;

    QModelIndex row = rows.first();
    if (!row.parent().isValid())
        return;

    m_model->resetShortcut(m_proxy->mapToSource(row));
    ui->resetButton->setEnabled(false);
}

void CommandsSettingsWidget::resetAll()
{
    for (int i = 0; i < m_model->rowCount(); i++) {
        QModelIndex parent = m_model->index(i);
        for (int j = 0; j < m_model->rowCount(parent); j++) {
            QModelIndex index = m_model->index(j, 0, parent);
            m_model->resetShortcut(index);
        }
    }
}

void CommandsSettingsWidget::exportShortcuts()
{
    QString filePath = QFileDialog::getSaveFileName(this,
                                                    tr("Export shortcuts"),
                                                    QString(),
                                                    tr("Keyboard mappings scheme (*.kms)"));
    if (filePath.isEmpty())
        return;

    QFile file(filePath);
    if (!file.open(QFile::WriteOnly)) {
        QMessageBox::warning(this,
                             tr("Warning"),
                             tr("Unable to open file %1 for writing.").arg(QFileInfo(filePath).fileName()),
                             QMessageBox::Close);
        return;
    }
    m_model->exportShortcuts(&file);
}

void CommandsSettingsWidget::importShortcuts()
{
    QString filePath = QFileDialog::getOpenFileName(this,
                                                    tr("Import shortcuts"),
                                                    QString(),
                                                    tr("Keyboard mappings scheme (*.kms)"));
    if (filePath.isEmpty())
        return;

    QFile file(filePath);
    if (!file.open(QFile::ReadOnly)) {
        QMessageBox::warning(this,
                             tr("Warning"),
                             tr("Unable to open file %1 for reading.").arg(QFileInfo(filePath).fileName()),
                             QMessageBox::Close);
        return;
    }

    if (!m_model->importShortcuts(&file)) {
        QMessageBox::warning(this,
                             tr("Warning"),
                             tr("Unable to import shortcuts from file %1.").arg(QFileInfo(filePath).fileName()),
                             QMessageBox::Close);
        return;
    }
    ui->view->expandAll();
}
