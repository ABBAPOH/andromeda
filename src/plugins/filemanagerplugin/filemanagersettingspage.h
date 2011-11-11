#ifndef FILEMANAGERSETTINGSPAGE_H
#define FILEMANAGERSETTINGSPAGE_H

#include <coreplugin/isettingspage.h>
#include <QtCore/QString>
#include <QtGui/QIcon>
#include <QtGui/QWidget>

namespace FileManagerPlugin {

class FileManagerSettingsPage : public CorePlugin::ISettingsPage
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

} // namespace FileManagerPlugin

#endif // FILEMANAGERSETTINGSPAGE_H
