#include "filemanagerdocument.h"

#include <QtCore/QDir>

#if QT_VERSION >= 0x050000
#include <QtWidgets/QFileIconProvider>
#else
#include <QtGui/QFileIconProvider>
#endif

#include "filemanagereditor_p.h"

using namespace Parts;
using namespace FileManager;

FileManagerDocument::FileManagerDocument(QObject *parent) :
    AbstractDocument(parent),
    m_history(new FileManagerEditorHistory(this))
{
}

/*!
  \reimp
*/
IHistory * FileManagerDocument::history() const
{
    return m_history;
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

/*!
    \class FileManagerDocumentFactory
*/

FileManagerDocumentFactory::FileManagerDocumentFactory(QObject *parent) :
    AbstractDocumentFactory("FileManager", parent)
{
}

/*!
    \reimp
*/
QString FileManagerDocumentFactory::name() const
{
    return tr("File manager");
}

/*!
    \reimp
*/
QIcon FileManagerDocumentFactory::icon() const
{
    return QFileIconProvider().icon(QFileIconProvider::Folder);
}

/*!
    \reimp
*/
QStringList FileManagerDocumentFactory::mimeTypes() const
{
    return QStringList() << QLatin1String("inode/directory");
}

/*!
    \reimp
*/
AbstractDocument * FileManagerDocumentFactory::createDocument(QObject *parent)
{
    return new FileManagerDocument(parent);
}
