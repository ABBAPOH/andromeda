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
    this->model = model;
    mapper->setModel(model);
    mapper->setRootIndex(model->index(0,0));
    mapper->addMapping(ui->label_Name, 0, "text");
    mapper->addMapping(ui->label_Version, 3, "text");
    mapper->addMapping(ui->label_CompatibilityVersion, 4, "text");
    mapper->addMapping(ui->label_Vendor, 5, "text");
    mapper->addMapping(ui->label_Url, 6, "text");
    mapper->addMapping(ui->label_Location, 7, "text");
    mapper->addMapping(ui->textBrowser_Description, 8, "plainText");
    mapper->addMapping(ui->label_Copyright, 9, "text");
    mapper->addMapping(ui->textBrowser_License, 10, "plainText");
    mapper->addMapping(ui->textBrowser_Dependencies, 11, "plainText");
}

void FullPluginView::setIndex(const QModelIndex &index)
{
    mapper->setRootIndex(index.parent());
    mapper->setCurrentModelIndex(index);
}



