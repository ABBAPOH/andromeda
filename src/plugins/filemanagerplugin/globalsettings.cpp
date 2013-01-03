#include "globalsettings.h"
#include "ui_globalsettings.h"

#include <QtCore/QSettings>
#include <QtCore/QSignalMapper>
#include <QtGui/QFileIconProvider>
#include <QtGui/QLabel>

#include <FileManager/FileManagerSettings>
#include <FileManager/NavigationModel>
#include <FileManager/NavigationPanelSettings>

using namespace FileManager;

/*!
    \class FileManager::GlobalSettingsWidget

    \brief GlobalSettingsWidget is a widget for representing global settings
    for a file manager.

    That includes settings for enabling or disabling confirmations when
    removing files and changing file extensions.

    \image html filemanager_globalsettings.png
*/

/*!
    Creates GlobalSettingsWidget with the given \a parent.
*/
GlobalSettingsWidget::GlobalSettingsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GlobalSettingsWidget),
    m_settings(new QSettings(this)),
    m_fileManagerSettings(FileManagerSettings::globalSettings())
{
    ui->setupUi(this);

    m_settings->beginGroup(QLatin1String("fileManager"));

    ui->fileRemoveCheckBox->setChecked(m_fileManagerSettings->warnOnFileRemove());
    ui->extensionChangeCheckBox->setChecked(m_fileManagerSettings->warnOnExtensionChange());

    connect(ui->fileRemoveCheckBox, SIGNAL(toggled(bool)), this, SLOT(onFileRemoveToggled(bool)));
    connect(ui->extensionChangeCheckBox, SIGNAL(toggled(bool)), this, SLOT(onExtensionChangeToggled(bool)));
}

/*!
    Destroys GlobalSettingsWidget.
*/
GlobalSettingsWidget::~GlobalSettingsWidget()
{
    delete ui;
}

void GlobalSettingsWidget::onFileRemoveToggled(bool state)
{
    m_settings->setValue(QLatin1String("warnOnFileRemove"), state);
    m_fileManagerSettings->setWarnOnFileRemove(state);
}

void GlobalSettingsWidget::onExtensionChangeToggled(bool state)
{
    m_settings->setValue(QLatin1String("warnOnExtensionChange"), state);
    m_fileManagerSettings->setWarnOnExtensionChange(state);
}

/*!
    \class FileManager::GlobalSettingsPage

    \brief This class is a GuiSystem::SettingsPage implementation for the GlobalSettingsWidget.
*/

/*!
    Creates GlobalSettingsPage with the given \a parent.
*/
GlobalSettingsPage::GlobalSettingsPage(QObject *parent) :
    SettingsPage("Global", "FileManager", parent)
{
}

/*!
    \reimp
*/
QString GlobalSettingsPage::name() const
{
    return tr("Global");
}

QString GlobalSettingsPage::categoryName() const
{
    return tr("File manager");
}

/*!
    \reimp
*/
QIcon GlobalSettingsPage::icon() const
{
    return QFileIconProvider().icon(QFileIconProvider::Folder);
}

/*!
    \reimp
*/
QIcon GlobalSettingsPage::categoryIcon() const
{
    return icon();
}

/*!
    \reimp
*/
QWidget *GlobalSettingsPage::createPage(QWidget *parent)
{
    return new GlobalSettingsWidget(parent);
}
