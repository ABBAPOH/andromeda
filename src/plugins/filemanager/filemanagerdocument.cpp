#include "filemanagerdocument.h"

#include <QtCore/QDir>
#include <QtGui/QFileIconProvider>

using namespace GuiSystem;
using namespace FileManager;

FileManagerDocument::FileManagerDocument(QObject *parent) :
    AbstractDocument(parent)
{
}

QString FileManagerDocument::currentPath() const
{
    return m_currentPath;
}

static QString getTitle(const QFileInfo &fi)
{
    QString path = fi.absoluteFilePath();
    if (path.endsWith(QLatin1Char('/')))
        path = path.left(path.length() - 1);

    if (path.isEmpty())
        return QLatin1String("/");

    if (fi.exists()) {
        QString result = fi.fileName();
        if (result.isEmpty())
            result = fi.filePath();
        return result;
    }
    return QString();
}

void FileManagerDocument::setCurrentPath(const QString &path)
{
    QString cleanPath = QDir::cleanPath(path);

    if (m_currentPath == cleanPath)
        return;

    m_currentPath = cleanPath;

    QFileInfo info(m_currentPath);
    setIcon(QFileIconProvider().icon(info));
    setTitle(getTitle(info));
    setUrl(QUrl::fromLocalFile(cleanPath));

    emit currentPathChanged(cleanPath);
}

/*!
    \reimp
*/
bool FileManagerDocument::openUrl(const QUrl &url)
{
    QString path = url.toLocalFile();
    path = QDir::cleanPath(path);
#ifdef Q_OS_WIN
    if (path == QLatin1String("/"))
        path.clear();
#endif

    setCurrentPath(path);
    return true;
}
