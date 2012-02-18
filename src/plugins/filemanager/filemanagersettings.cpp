#include "filemanagersettings.h"
#include "filemanagersettings_p.h"

#include "filemanagerwidget.h"
#include "filemanagerwidget_p.h"

using namespace FileManager;

void FileManagerSettingsPrivate::addWidget(FileManagerWidget *widget)
{
    widgets.append(widget);
}

void FileManagerSettingsPrivate::removeWidget(FileManagerWidget *widget)
{
    widgets.removeAll(widget);
}

FileManagerSettings::FileManagerSettings() :
    d_ptr(new FileManagerSettingsPrivate)
{
    Q_D(FileManagerSettings);

    d->flow = LeftToRight;
#ifdef Q_OS_MAC
    d->iconSizes[IconView] = QSize(64, 64);
    d->gridSize = QSize(128, 128);
#else
    d->iconSizes[IconView] = QSize(32, 32);
    d->gridSize = QSize(96, 96);
#endif
    d->iconSizes[ColumnView] = QSize(16, 16);
    d->iconSizes[TreeView] = QSize(16, 16);
    d->itemsExpandable = true;
}

FileManagerSettings *FileManagerSettings::globalSettings()
{
    static FileManagerSettings *instance = 0;
    if (!instance)
        instance = new FileManagerSettings();
    return instance;
}

QSize FileManagerSettings::gridSize() const
{
    Q_D(const FileManagerSettings);

    return d->gridSize;
}

void FileManagerSettings::setGridSize(QSize size)
{
    Q_D(FileManagerSettings);

    if (d->gridSize == size)
        return;

    d->gridSize = size;
    foreach (FileManagerWidget *widget, d->widgets) {
        widget->setGridSize(size);
    }
}

FileManagerSettings::Flow FileManagerSettings::flow() const
{
    Q_D(const FileManagerSettings);

    return d->flow;
}

void FileManagerSettings::setFlow(FileManagerSettings::Flow flow)
{
    Q_D(FileManagerSettings);

    if (d->flow == flow)
        return;

    d->flow = flow;
    foreach (FileManagerWidget *widget, d->widgets) {
        widget->setFlow((FileManagerWidget::Flow)flow);
    }
}

QSize FileManagerSettings::iconSize(FileManagerSettings::ViewMode mode) const
{
    Q_D(const FileManagerSettings);

    if (mode < 0 || mode >= ViewCount)
        return QSize();

    return d->iconSizes[mode];
}

void FileManagerSettings::setIconSize(FileManagerSettings::ViewMode mode, QSize size)
{
    Q_D(FileManagerSettings);

    if (mode < 0 || mode >= ViewCount)
        return;

    if (d->iconSizes[mode] == size)
        return;

    d->iconSizes[mode] = size;

    foreach (FileManagerWidget *widget, d->widgets) {
        widget->setIconSize((FileManagerWidget::ViewMode)mode, size);
    }
}

bool FileManagerSettings::itemsExpandable() const
{
    Q_D(const FileManagerSettings);

    return d->itemsExpandable;
}

void FileManagerSettings::setItemsExpandable(bool expandable)
{
    Q_D(FileManagerSettings);

    if (d->itemsExpandable == expandable)
        return;

    d->itemsExpandable = expandable;
    foreach (FileManagerWidget *widget, d->widgets) {
        widget->setItemsExpandable(expandable);
    }
}
