#include "pluginview.h"
#include "ui_pluginview.h"

#include "fullpluginview.h"
#include "pluginviewmodel.h"

using namespace ExtensionSystem;

/*!
    \class ExtensionSystem::PluginView

    \brief PluginView is a dialog for displaying application's plugins.

    This class shows list of available plugins in a tree structure and allows
    to view specified plugin in a detailed form using ExtensionSystem::FullPluginView.
*/

/*!
    Creates PluginView with given \a parent.
*/
PluginView::PluginView(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PluginView)
{
    ui->setupUi(this);

    PluginViewModel* model = new PluginViewModel(this);
    ui->treeView->setModel(model);
    ui->treeView->setColumnWidth(0, 190);
    ui->treeView->expandAll();

    ui->treeView->hideColumn(4);
    ui->treeView->hideColumn(6);
    ui->treeView->hideColumn(7);
    ui->treeView->hideColumn(8);
    ui->treeView->hideColumn(9);
    ui->treeView->hideColumn(10);
    ui->treeView->hideColumn(11);

    ui->pushButton_More->setEnabled(false);

    connect(ui->pushButton_More, SIGNAL(clicked()), SLOT(showFullInfo()));
    connect(ui->treeView, SIGNAL(doubleClicked(QModelIndex)), SLOT(showFullInfo(QModelIndex)));
    connect(ui->treeView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            SLOT(onSelectionChanged()));
}

/*!
    Destroys PluginView.
*/
PluginView::~PluginView()
{
    delete ui;
}

/*!
    \internal
*/
void PluginView::showFullInfo(const QModelIndex & i)
{
    FullPluginView fullPluginView(this);
    fullPluginView.setModel(qobject_cast<PluginViewModel*>(ui->treeView->model()));

    QModelIndex index;
    if (!i.isValid()) {
        QModelIndexList rows = ui->treeView->selectionModel()->selectedRows();
        if (rows.isEmpty())
            return;
        index = rows.first();
    } else {
        index = i;
    }
    if (index.parent() == QModelIndex()) { // we're at top level index

        return;
    }

    fullPluginView.setIndex(index);
    fullPluginView.exec();
}

/*!
    \internal
*/
void PluginView::onSelectionChanged()
{
    QItemSelectionModel *model = ui->treeView->selectionModel();
    if (model) {
        // check if plugin selected
        bool enable = model->currentIndex().parent().isValid();
        ui->pushButton_More->setEnabled(enable);
    }
}
