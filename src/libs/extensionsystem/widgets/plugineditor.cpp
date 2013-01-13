#include "plugineditor.h"
#include "ui_plugineditor.h"

#include <QtGui/QStringListModel>
#include <ExtensionSystem/MutablePluginSpec>

using namespace ExtensionSystem;

/*!
    \class ExtensionSystem::PluginEditor
    \brief PluginEditor is a widget for editing plugin specs.
*/

/*!
    Creates PluginEditor with given \a parent.
*/
PluginEditor::PluginEditor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PluginEditor)
{
    ui->setupUi(this);
    dependenciesModel = new QStringListModel();
    ui->listView_Dependencies->setModel(dependenciesModel);
}

/*!
    Destroys PluginEditor.
*/
PluginEditor::~PluginEditor()
{
    delete ui;
}

/*!
    Returns current plugin.
*/
PluginSpec * PluginEditor::pluginSpec()
{
    return m_pluginSpec;
}

/*!
    Sets current plugin.
*/
void PluginEditor::setPluginSpec(MutablePluginSpec *spec)
{
    if (m_pluginSpec != spec) {
        m_pluginSpec = spec;

        reset();
    }
}

/*!
    Applies all data from widget to the current plugin.
*/
void PluginEditor::apply()
{
    m_pluginSpec->setName(ui->lineEdit_Name->text());
    m_pluginSpec->setVersion(Version::fromString(ui->lineEdit_Version->text()));
    m_pluginSpec->setCompatibilityVersion(Version::fromString(ui->lineEdit_CompatibilityVersion->text()));
    m_pluginSpec->setVendor(ui->lineEdit_Vendor->text());
    m_pluginSpec->setUrl(ui->lineEdit_Url->text());
    m_pluginSpec->setCopyright(ui->lineEdit_Copyright->text());
    m_pluginSpec->setDescription(ui->textBrowser_Description->toPlainText());
    m_pluginSpec->setLicense(ui->textBrowser_License->toPlainText());

    QStringList dependecyList;
    foreach (const PluginDependency &dep, m_pluginSpec->dependencies()) {
        dependecyList.append(QString(QLatin1String("%1, %2")).arg(dep.name()).arg(dep.version().toString()));
    }
    dependenciesModel->setStringList(dependecyList);
}

/*!
    Resets all data in widget to the data in current plugin.
*/
void PluginEditor::reset()
{
    ui->lineEdit_Name->setText(m_pluginSpec->name());
    ui->lineEdit_Version->setText(m_pluginSpec->version().toString());
    ui->lineEdit_CompatibilityVersion->setText(m_pluginSpec->compatibilityVersion().toString());
    ui->lineEdit_Vendor->setText(m_pluginSpec->vendor());
    ui->lineEdit_Url->setText(m_pluginSpec->url());
    ui->lineEdit_Copyright->setText(m_pluginSpec->copyright());
    ui->textBrowser_Description->setPlainText(m_pluginSpec->description());
    ui->textBrowser_License->setPlainText(m_pluginSpec->license());
//    ui->textBrowser_Dependencies->setText(m_pluginSpec->license());
    QList<PluginDependency> dependencies;
    foreach (const QString &depString, dependenciesModel->stringList()) {
        QStringList list = depString.split(QLatin1String(", "));
        if (list.count() == 2) {
            dependencies.append(PluginDependency(list.at(0), list.at(1)));
        }
    }
}
