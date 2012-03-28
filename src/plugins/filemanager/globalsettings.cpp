#include "globalsettings.h"
#include "ui_globalsettings.h"

#include <QtCore/QSettings>
#include <QtCore/QSignalMapper>
#include <QtGui/QFileIconProvider>
#include <QtGui/QLabel>

#include "filemanagersettings.h"
#include "navigationmodel.h"
#include "navigationpanelsettings.h"

using namespace FileManager;

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

GlobalSettingsPage::GlobalSettingsPage(QObject *parent) :
    SettingsPage("Global", "FileManager", parent)
{
}

QString GlobalSettingsPage::name() const
{
    return tr("Global");
}

QString GlobalSettingsPage::categoryName() const
{
    return tr("FileManager");
}

QIcon GlobalSettingsPage::icon() const
{
    return QFileIconProvider().icon(QFileIconProvider::Folder);
}

QWidget *GlobalSettingsPage::createPage(QWidget *parent)
{
    return new GlobalSettingsWidget(parent);
}

QIcon GlobalSettingsPage::categoryIcon() const
{
    return icon();
}
