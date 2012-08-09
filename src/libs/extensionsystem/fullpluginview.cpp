#include "fullpluginview.h"
#include "ui_fullpluginview.h"

#include <QtGui/QDataWidgetMapper>

#include "pluginviewmodel.h"

using namespace ExtensionSystem;

/*!
    \class ExtensionSystem::FullPluginView
    \brief FullPluginView provides detalied view of a single plugin.

    \image html fullpluginview.png
*/

/*!
    Creates FullPluginView with given \a parent.
*/
FullPluginView::FullPluginView(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FullPluginView),
    m_mapper(new QDataWidgetMapper(this)),
    m_model(0)
{
    ui->setupUi(this);
}

/*!
    Destroys FullPluginView.
*/
FullPluginView::~FullPluginView()
{
    delete ui;
}

/*!
    Returns current model.
*/
PluginViewModel * FullPluginView::model() const
{
    return m_model;
}

/*!
    Sets current model.
*/
void FullPluginView::setModel(PluginViewModel * model)
{
    if (m_model == model)
        return;

    m_model = model;
    m_mapper->clearMapping();
    m_mapper->setModel(m_model);
    if (m_model != 0) {
        m_mapper->setRootIndex(m_model->index(0,0));
        m_mapper->addMapping(ui->label_Name, 0, "text");
        m_mapper->addMapping(ui->label_Version, 3, "text");
        m_mapper->addMapping(ui->label_CompatibilityVersion, 4, "text");
        m_mapper->addMapping(ui->label_Vendor, 5, "text");
        m_mapper->addMapping(ui->label_Url, 6, "text");
        m_mapper->addMapping(ui->lineEdit_Location, 7, "text");
        m_mapper->addMapping(ui->textBrowser_Description, 8, "plainText");
        m_mapper->addMapping(ui->label_Copyright, 9, "text");
        m_mapper->addMapping(ui->textBrowser_License, 10, "plainText");
        m_mapper->addMapping(ui->textBrowser_Dependencies, 11, "plainText");
    }
}

/*!
    Sets current model \a index corresponding to a specified plugin.
*/
void FullPluginView::setIndex(const QModelIndex &index)
{
    m_mapper->setRootIndex(index.parent());
    m_mapper->setCurrentModelIndex(index);
}
