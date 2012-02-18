#ifndef FILEMANAGERSETTINGSPAGE_H
#define FILEMANAGERSETTINGSPAGE_H

#include <guisystem/settingspage.h>
#include <QtCore/QString>
#include <QtGui/QIcon>
#include <QtGui/QWidget>

namespace FileManager {

class FileManagerSettingsPage : public GuiSystem::SettingsPage
{
    Q_OBJECT

public:
    explicit FileManagerSettingsPage(QObject *parent = 0);

    QString name() const;
    QIcon icon() const;

    QString categoryName() const;
    QIcon categoryIcon() const;

    QWidget *createPage(QWidget *parent);
};

} // namespace FileManager

#endif // FILEMANAGERSETTINGSPAGE_H
