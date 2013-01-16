#ifndef FILEMANAGERDOCUMENT_H
#define FILEMANAGERDOCUMENT_H

#include <GuiSystem/AbstractDocument>
#include <GuiSystem/AbstractDocumentFactory>

namespace FileManager {

class FileManagerEditorHistory;

class FileManagerDocument : public GuiSystem::AbstractDocument
{
    Q_OBJECT
    Q_DISABLE_COPY(FileManagerDocument)

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

class FileManagerDocumentFactory : public GuiSystem::AbstractDocumentFactory
{
    Q_OBJECT
    Q_DISABLE_COPY(FileManagerDocumentFactory)

public:
    explicit FileManagerDocumentFactory(QObject *parent = 0);

    QString name() const;
    QIcon icon() const;
    QStringList mimeTypes() const;

protected:
    GuiSystem::AbstractDocument *createDocument(QObject *parent);
};

} // namespace FileManager

#endif // FILEMANAGERDOCUMENT_H
