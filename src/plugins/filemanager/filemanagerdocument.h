#ifndef FILEMANAGERDOCUMENT_H
#define FILEMANAGERDOCUMENT_H

#include <guisystem/abstractdocument.h>

namespace FileManager {

class FileManagerDocument : public GuiSystem::AbstractDocument
{
    Q_OBJECT
    Q_PROPERTY(QString currentPath READ currentPath WRITE setCurrentPath NOTIFY currentPathChanged)
public:
    explicit FileManagerDocument(QObject *parent = 0);

    QString currentPath() const;

public slots:
    void setCurrentPath(const QString &path);

signals:
    void currentPathChanged(const QString &path);

protected:
    bool openUrl(const QUrl &url);

private:
    QString m_currentPath;
};

} // namespace FileManager

#endif // FILEMANAGERDOCUMENT_H
