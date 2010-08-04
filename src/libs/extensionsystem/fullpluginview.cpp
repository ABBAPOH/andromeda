#include "fullpluginview.h"
#include "ui_fullpluginview.h"

#include <QtGui/QDataWidgetMapper>

#include "pluginviewmodel.h"

using namespace ExtensionSystem;

FullPluginView::FullPluginView(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FullPluginView),
    mapper(new QDataWidgetMapper(this))
{
    ui->setupUi(this);
}

FullPluginView::~FullPluginView()
{
    delete ui;
}

void FullPluginView::setModel(PluginViewModel * model)
{
    qDebug("FullPluginView::setModel");
    this->model = model;
    mapper->setModel(model);
    mapper->setRootIndex(model->index(0,0));
    mapper->addMapping(ui->label_Name, 0, "text");
    mapper->addMapping(ui->label_Version, 1, "text");
    mapper->addMapping(ui->label_CompatibilityVersion, 2, "text");
    mapper->addMapping(ui->label_Vendor, 3, "text");
}

void FullPluginView::setIndex(const QModelIndex &index)
{
    mapper->setRootIndex(index.parent());
    mapper->setCurrentModelIndex(index);
}



