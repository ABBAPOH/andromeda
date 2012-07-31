#include "viewmodessettings.h"

#include "ui_viewmodessettings.h"

#include <guisystem/settingspage.h>
#include <QtGui/QFileIconProvider>
#include <QtGui/QLabel>
#include <QtCore/QSettings>
#include <QtCore/QSignalMapper>

#include "filemanagersettings.h"
#include "navigationmodel.h"
#include "navigationpanelsettings.h"

using namespace FileManager;

/*!
    \class FileManager::ViewModesSettingsWidget

    ViewModesSettingsWidget is a widget for representing settings for file
    manager view modes. That includes standard location settings, icon and grid
    size settings, flow settings for icon mode and setting that holds wheter
    items are expandable in a tree view mode, or not.
*/

/*!
    Creates ViewModesSettingsWidget with the given \a parent.
*/
ViewModesSettingsWidget::ViewModesSettingsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ViewModesSettingsWidget),
    m_settings(new QSettings(this)),
    m_fileManagerSettings(FileManagerSettings::globalSettings()),
    m_panelSettings(NavigationPanelSettings::globalSettings())
{
    ui->setupUi(this);

    m_settings->beginGroup(QLatin1String("fileManager"));

    setupLeftPanel();
    setupIconSize();
    setupGridSize();
    setupFlow();
    setupTreeView();
}

/*!
    Destroys ViewModesSettingsWidget.
*/
ViewModesSettingsWidget::~ViewModesSettingsWidget()
{
    delete ui;
}

void ViewModesSettingsWidget::onChecked(bool checked)
{
    int flags = m_panelSettings->standardLocations();

    if (checked)
        flags = flags | sender()->property("flag").toInt();
    else
        flags = flags & ~sender()->property("flag").toInt();

    m_panelSettings->setStandardLocations(NavigationModel::StandardLocations(flags));
}

void ViewModesSettingsWidget::onIconSizeChanged(int value)
{
    value *= 4;
    QSize size(value, value);
    ui->iconSizeLabel->setText(tr("Icon size: %1x%2").arg(value).arg(value));

    m_settings->setValue(QLatin1String("iconSize"), size);
    m_fileManagerSettings->setIconSize(FileManagerSettings::IconView, size);

    onGridSizeChanged(ui->gridSizeSlider->value());
}

static int calcGridSize(int iconSize, int factor, bool iconMode)
{
    static const int minIconSize = 32;
    static const int minListSize = 2;

    return (iconSize + (iconMode ? minIconSize : minListSize))*(1.0 + factor/100.0) + 0.5;
}

static int calcGridFactor(int iconSize, int gridSize, bool iconMode)
{
    static const int minIconSize = 32;
    static const int minListSize = 2;

    return (100.0*gridSize)/(iconSize + (iconMode ? minIconSize : minListSize)) - 100 + 0.5;
}

void ViewModesSettingsWidget::onGridSizeChanged(int value)
{
    bool iconMode = ui->flowComboBox->currentIndex() == 0;
    int iconSize = ui->iconSizeSlider->value()*4;
    int gridSize = calcGridSize(iconSize, value, iconMode);

    QSize size(gridSize, gridSize);

    m_settings->setValue(QLatin1String("gridSize"), size);
    m_fileManagerSettings->setGridSize(size);
}

void ViewModesSettingsWidget::onFlowChanged(int value)
{
    m_settings->setValue(QLatin1String("flow"), value);
    m_fileManagerSettings->setFlow((FileManagerSettings::Flow)value);

    onGridSizeChanged(ui->gridSizeSlider->value());
}

void ViewModesSettingsWidget::onColumnIconSizeChanged(int value)
{
    value *= 4;
    QSize size(value, value);

    ui->columnIconSizeLabel->setText(tr("Icon size: %1x%2").arg(value).arg(value));
    m_fileManagerSettings->setIconSize(FileManagerSettings::ColumnView, size);
}

void ViewModesSettingsWidget::onTreeIconSizeChanged(int value)
{
    value *= 4;
    QSize size(value, value);

    ui->treeIconSizeLabel->setText(tr("Icon size: %1x%2").arg(value).arg(value));
    m_fileManagerSettings->setIconSize(FileManagerSettings::TreeView, size);
}

void ViewModesSettingsWidget::onItemsExpandableChecked(bool checked)
{
    m_settings->setValue(QLatin1String("itemsExpandable"), checked);
    m_fileManagerSettings->setItemsExpandable(checked);
}

void ViewModesSettingsWidget::setupLeftPanel()
{
    int flags = m_panelSettings->standardLocations();

    ui->applicationsCheckBox->setChecked(flags & NavigationModel::ApplicationsLocation);
    ui->desktopCheckBox->setChecked(flags & NavigationModel::DesktopLocation);
    ui->documentsCheckBox->setChecked(flags & NavigationModel::DocumentsLocation);
    ui->downloadsCheckBox->setChecked(flags & NavigationModel::DownloadsLocation);
    ui->moviesCheckBox->setChecked(flags & NavigationModel::MoviesLocation);
    ui->musicCheckBox->setChecked(flags & NavigationModel::MusicLocation);
    ui->picturesCheckBox->setChecked(flags & NavigationModel::PicturesLocation);
    ui->homeCheckBox->setChecked(flags & NavigationModel::HomeLocation);

    ui->applicationsCheckBox->setProperty("flag", NavigationModel::ApplicationsLocation);
    ui->desktopCheckBox->setProperty("flag", NavigationModel::DesktopLocation);
    ui->documentsCheckBox->setProperty("flag", NavigationModel::DocumentsLocation);
    ui->downloadsCheckBox->setProperty("flag", NavigationModel::DownloadsLocation);
    ui->moviesCheckBox->setProperty("flag", NavigationModel::MoviesLocation);
    ui->musicCheckBox->setProperty("flag", NavigationModel::MusicLocation);
    ui->picturesCheckBox->setProperty("flag", NavigationModel::PicturesLocation);
    ui->homeCheckBox->setProperty("flag", NavigationModel::HomeLocation);

    connect(ui->applicationsCheckBox, SIGNAL(toggled(bool)), SLOT(onChecked(bool)));
    connect(ui->desktopCheckBox, SIGNAL(toggled(bool)), SLOT(onChecked(bool)));
    connect(ui->documentsCheckBox, SIGNAL(toggled(bool)), SLOT(onChecked(bool)));
    connect(ui->downloadsCheckBox, SIGNAL(toggled(bool)), SLOT(onChecked(bool)));
    connect(ui->moviesCheckBox, SIGNAL(toggled(bool)), SLOT(onChecked(bool)));
    connect(ui->musicCheckBox, SIGNAL(toggled(bool)), SLOT(onChecked(bool)));
    connect(ui->picturesCheckBox, SIGNAL(toggled(bool)), SLOT(onChecked(bool)));
    connect(ui->homeCheckBox, SIGNAL(toggled(bool)), SLOT(onChecked(bool)));
}

void ViewModesSettingsWidget::setupIconSize()
{
    int iconSize = m_fileManagerSettings->iconSize(FileManagerSettings::IconView).height();
    int columnIconSize = m_fileManagerSettings->iconSize(FileManagerSettings::ColumnView).height();
    int treeIconSize = m_fileManagerSettings->iconSize(FileManagerSettings::TreeView).height();

    ui->iconSizeSlider->setValue(iconSize/4);
    ui->iconSizeLabel->setText(tr("Icon size: %1x%2").arg(iconSize).arg(iconSize));

    ui->columnIconSize->setValue(columnIconSize/4);
    ui->columnIconSizeLabel->setText(tr("Icon size: %1x%2").arg(columnIconSize).arg(columnIconSize));

    ui->treeIconSize->setValue(treeIconSize/4);
    ui->treeIconSizeLabel->setText(tr("Icon size: %1x%2").arg(treeIconSize).arg(treeIconSize));

    connect(ui->iconSizeSlider, SIGNAL(valueChanged(int)), SLOT(onIconSizeChanged(int)));
    connect(ui->columnIconSize, SIGNAL(valueChanged(int)), SLOT(onColumnIconSizeChanged(int)));
    connect(ui->treeIconSize, SIGNAL(valueChanged(int)), SLOT(onTreeIconSizeChanged(int)));
}

void ViewModesSettingsWidget::setupGridSize()
{
    int iconSize = m_fileManagerSettings->iconSize(FileManagerSettings::IconView).height();
    int gridSize = m_fileManagerSettings->gridSize().height();
    FileManagerSettings::Flow flow = m_fileManagerSettings->flow();

    int factor = calcGridFactor(iconSize, gridSize, flow == FileManagerSettings::LeftToRight);
    ui->gridSizeSlider->setValue(factor);

    connect(ui->gridSizeSlider, SIGNAL(valueChanged(int)), SLOT(onGridSizeChanged(int)));
}

void ViewModesSettingsWidget::setupFlow()
{
    FileManagerSettings::Flow flow = m_fileManagerSettings->flow();
    ui->flowComboBox->setCurrentIndex(flow);
    connect(ui->flowComboBox, SIGNAL(currentIndexChanged(int)), SLOT(onFlowChanged(int)));
}

void ViewModesSettingsWidget::setupTreeView()
{
    bool checked = m_settings->value(QLatin1String("itemsExpandable"), true).toBool();
    ui->itemsExpandable->setChecked(checked);
    connect(ui->itemsExpandable, SIGNAL(toggled(bool)), SLOT(onItemsExpandableChecked(bool)));
}

/*!
    \class FileManager::ViewModesSettingsPage

    ViewModesSettingsPage is an implementation of the GuiSystem::SettingsPage
    interface for the ViewModesSettingsWidget.
*/

/*!
    Creates ViewModesSettingsPage with the given \a parent.
*/
ViewModesSettingsPage::ViewModesSettingsPage(QObject *parent) :
    SettingsPage("View Modes", "FileManager", parent)
{
}

/*!
    \reimp
*/
QString ViewModesSettingsPage::name() const
{
    return tr("View Modes");
}

/*!
    \reimp
*/
QString ViewModesSettingsPage::categoryName() const
{
    return tr("File manager");
}

/*!
    \reimp
*/
QIcon ViewModesSettingsPage::icon() const
{
    return QFileIconProvider().icon(QFileIconProvider::Folder);
}

/*!
    \reimp
*/
QIcon ViewModesSettingsPage::categoryIcon() const
{
    return icon();
}

/*!
    \reimp
*/
QWidget *ViewModesSettingsPage::createPage(QWidget *parent)
{
    return new ViewModesSettingsWidget(parent);
}
