#include "settingswidget.h"
#include "ui_settingswidget.h"

#include "settingsmodel.h"

using namespace Andromeda;

/*!
    \class Andromeda::SettingsWidget

    \brief SettingsWidget is a widget that displays applications settings in an
    item view.

    \image html settingswidget.png
*/

/*!
    Creates SettingsWidget with the given \a parent.
*/
SettingsWidget::SettingsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingsWidget),
    m_model(0)
{
    ui->setupUi(this);

    m_closeAction = new QAction(tr("Close"), this);
    m_closeAction->setShortcut(QKeySequence::Close);
    m_closeAction->setShortcutContext(Qt::WindowShortcut);
    addAction(m_closeAction);

    connect(ui->removeButton, SIGNAL(clicked()), SLOT(remove()));
    connect(ui->refreshButton, SIGNAL(clicked()), SLOT(refresh()));
    connect(m_closeAction, SIGNAL(triggered()), SLOT(close()));
}

/*!
    Destroys SettingsWidget.
*/
SettingsWidget::~SettingsWidget()
{
    delete ui;
}

/*!
    Returns currently set model. By default, no model is set.
*/
SettingsModel * SettingsWidget::model() const
{
    return m_model;
}

/*!
    Sets current model.
*/
void SettingsWidget::setModel(SettingsModel *model)
{
    if (m_model == model)
        return;

    m_model = model;

    ui->treeView->setModel(model);

//    ui->treeView->expandAll();
#if QT_VERSION >= 0x050000
    ui->treeView->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->treeView->header()->setSectionResizeMode(2, QHeaderView::Stretch);
#else
    ui->treeView->header()->setResizeMode(0, QHeaderView::Stretch);
    ui->treeView->header()->setResizeMode(2, QHeaderView::Stretch);
#endif

    ui->removeButton->setVisible(!m_model->readOnly());
    ui->removeAllButton->setVisible(!m_model->readOnly());
}

void SettingsWidget::remove()
{
    if (!m_model)
        return;

    QModelIndex index = selectedRow();
    if (!index.isValid())
        return;

    m_model->removeRow(index.row(), index.parent());
}

void SettingsWidget::removeAll()
{
    if (!m_model)
        return;

    m_model->clear();
}

void SettingsWidget::refresh()
{
    if (!m_model)
        return;

    m_model->refresh();
}

QModelIndex SettingsWidget::selectedRow() const
{
    QModelIndexList indexes = ui->treeView->selectionModel()->selectedRows();

    if (indexes.isEmpty())
        return QModelIndex();

    return indexes.first();
}
