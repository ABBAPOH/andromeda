#ifndef FILEMANAGERSETTINGS_H
#define FILEMANAGERSETTINGS_H

#include <QtCore/QSize>

namespace FileManagerPlugin {

class FileManagerSettingsPrivate;
class FileManagerSettings
{
    Q_DECLARE_PRIVATE(FileManagerSettings)

public:
    static FileManagerSettings *globalSettings();

    enum Flow { LeftToRight = 0, TopToBottom = 1 };
    enum ViewMode { IconView = 0, ColumnView = 1, TreeView = 3, ViewCount };

    QSize gridSize() const;
    void setGridSize(QSize size);

    Flow flow() const;
    void setFlow(Flow flow);

    QSize iconSize(ViewMode mode) const;
    void setIconSize(ViewMode mode, QSize size);

protected:
    FileManagerSettings();

    FileManagerSettingsPrivate *d_ptr;

    friend class FileManagerWidget;
};

} // namespace FileManagerPlugin

#endif // FILEMANAGERSETTINGS_H
