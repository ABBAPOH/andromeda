#include "pluginview.h"
#include "ui_pluginview.h"

#include "fullpluginview.h"
#include "pluginviewmodel.h"

using namespace ExtensionSystem;

PluginView::PluginView(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PluginView),
    m_fullPluginView(new FullPluginView(this))
{
    ui->setupUi(this);

    PluginViewModel* model = new PluginViewModel(this);
    ui->treeView->setModel(model);
    ui->treeView->setColumnWidth(0, 190);
    m_fullPluginView->setModel(model);

    ui->treeView->hideColumn(4);
    ui->treeView->hideColumn(6);
    ui->treeView->hideColumn(7);
    ui->treeView->hideColumn(8);
    ui->treeView->hideColumn(9);
    ui->treeView->hideColumn(10);
    ui->treeView->hideColumn(11);

    connect(ui->pushButton_More, SIGNAL(clicked()), SLOT(showFullInfo()));
    connect(ui->treeView, SIGNAL(doubleClicked(QModelIndex)), SLOT(showFullInfo(QModelIndex)));
}

PluginView::~PluginView()
{
    delete ui;
}

void PluginView::showFullInfo(const QModelIndex & i)
{
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
    m_fullPluginView->setIndex(index);
    m_fullPluginView->exec();
}
