#include "settingswidget.h"
#include "ui_settingswidget.h"

#include "settingsmodel.h"

/*!
    \class SettingsWidget

    \image html settingswidget.png
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

SettingsWidget::~SettingsWidget()
{
    delete ui;
}

SettingsModel * SettingsWidget::model() const
{
    return m_model;
}

void SettingsWidget::setModel(SettingsModel *model)
{
    if (m_model == model)
        return;

    m_model = model;

    ui->treeView->setModel(model);

//    ui->treeView->expandAll();
    ui->treeView->header()->setResizeMode(0, QHeaderView::Stretch);
    ui->treeView->header()->setResizeMode(2, QHeaderView::Stretch);

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
