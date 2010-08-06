#include "pluginview.h"
#include "ui_pluginview.h"

#include "pluginmanager.h"
#include "pluginviewmodel.h"
#include "fullpluginview.h"

#include <QtCore/QModelIndex>
using namespace ExtensionSystem;

PluginView::PluginView(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PluginView),
    fullPluginView(new FullPluginView(this))
{
    ui->setupUi(this);
    PluginViewModel* model = new PluginViewModel(this);
    ui->treeView->setModel(model);
    ui->treeView->setColumnWidth(0, 200);
    fullPluginView->setModel(model);

    connect(ui->pushButton_More, SIGNAL(clicked()), SLOT(showFullInfo()));
//    connect(ui->treeView, SIGNAL(doubleClicked(QModelIndex)), SLOT(showFullInfo(QModelIndex)));
}

PluginView::~PluginView()
{
    delete ui;
}
#include <QDebug>
void PluginView::showFullInfo(const QModelIndex & i)
{
    QModelIndex index;
    qDebug() << i;
    if (!i.isValid()) {
        QModelIndexList rows = ui->treeView->selectionModel()->selectedRows();
        if (rows.isEmpty())
            return;
        index = rows.first();
    } else {
        index = i;
    }
            qDebug() << index;
    if (index.parent() == QModelIndex()) { // we're at top level index

        return;
    }
    fullPluginView->setIndex(index);
    fullPluginView->exec();
}
