#ifndef FILEMANAGERDOCUMENT_H
#define FILEMANAGERDOCUMENT_H

#include <GuiSystem/AbstractDocument>

namespace FileManager {

class FileManagerEditorHistory;

class FileManagerDocument : public GuiSystem::AbstractDocument
{
    Q_OBJECT
    Q_PROPERTY(QString currentPath READ currentPath WRITE setCurrentPath NOTIFY currentPathChanged)
public:
    explicit FileManagerDocument(QObject *parent = 0);

    GuiSystem::IHistory *history() const;
    FileManagerEditorHistory *fmhistory() const { return m_history; }

    QString currentPath() const;

public slots:
    void setCurrentPath(const QString &path);

signals:
    void currentPathChanged(const QString &path);

protected:
    bool openUrl(const QUrl &url);

private:
    QString m_currentPath;
    FileManagerEditorHistory *m_history;
};

} // namespace FileManager

#endif // FILEMANAGERDOCUMENT_H
