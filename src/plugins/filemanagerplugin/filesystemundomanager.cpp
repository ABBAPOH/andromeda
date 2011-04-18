#include "filesystemundomanager.h"

#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtGui/QApplication>
#include <QtGui/QUndoStack>
#include <IO>

using namespace IO;
using namespace FileManagerPlugin;

QUndoStack *m_undoStack = 0;

FileSystemUndoManager::FileSystemUndoManager(QObject *parent) :
    QObject(parent)
{
    if (!m_undoStack) {
        m_undoStack = new QUndoStack(qApp);
    }
}

QUndoStack * FileSystemUndoManager::undoStack() const
{
    return m_undoStack;
}

void CopyCommand::setDestination(const QString &path)
{
    m_destinationPath = path;
}

void CopyCommand::setSourcePaths(const QStringList &paths)
{
    m_sourcePaths = paths;
}

void CopyCommand::appendSourcePath(const QString &path)
{
    m_sourcePaths.append(path);
}

void CopyCommand::redo()
{
    FileCopyManager * manager = FileCopyManager::instance();
    QtFileCopier * copier = manager->copier();

    foreach (const QString &filePath, m_sourcePaths) {
        QFileInfo info(filePath);

        // TODO: give different names when copying to same dir
        if (info.isDir()) {
            copier->copyDirectory(filePath, m_destinationPath, QtFileCopier::NonInteractive);
        } else {
            copier->copy(filePath, m_destinationPath, QtFileCopier::NonInteractive);
        }
    }
}

void CopyCommand::undo()
{
    QDir dir(m_destinationPath);
    foreach (const QString &filePath, m_sourcePaths) {
        QFileInfo info(filePath);
        QString targetPath = dir.absolutePath() + '/' + info.fileName();
        QFile::remove(targetPath);
    }
}

void MoveCommand::redo()
{
    FileCopyManager * manager = FileCopyManager::instance();
    QtFileCopier * copier = manager->copier();

    m_destinationPaths.clear();
    foreach (const QString &filePath, m_sourcePaths) {
        QFileInfo info(filePath);

        // TODO: give different names when copying to same dir
        if (info.isDir()) {
            copier->moveDirectory(filePath, m_destinationPath);
            m_destinationPaths.append(m_destinationPath + QDir::separator() + info.fileName());
        } else {
            int id = copier->move(filePath, m_destinationPath);
            m_destinationPaths.append(copier->destinationFilePath(id));
        }
    }
}

void MoveCommand::undo()
{
    FileCopyManager * manager = FileCopyManager::instance();
    QtFileCopier * copier = manager->copier();

    int i = 0;
    foreach (const QString &filePath, m_destinationPaths) {
        QFileInfo info(filePath);
        QFileInfo sourceInfo(m_sourcePaths[i]);
        QDir dir = sourceInfo.dir();

        // TODO: give different names when copying to same dir
        if (info.isDir()) {
            copier->moveDirectory(filePath, dir.absoluteFilePath(info.fileName()));
        } else {
            copier->move(filePath, dir.absoluteFilePath(info.fileName()));
        }
        i++;
    }
}

void RenameCommand::setPath(const QString &path)
{
    m_path = path;
}

void RenameCommand::setNewName(const QString &name)
{
    m_newName = name;
}

void RenameCommand::redo()
{
    QFile(m_path).rename(m_newName);
}

void RenameCommand::undo()
{
    QFileInfo info(m_path);
    QString newPath = info.dir().absoluteFilePath(m_newName);
    QFile(newPath).rename(info.fileName());
}

void LinkCommand::redo()
{
    bool result = true;
    for (int i = 0; i < m_paths.size(); i++) {
        result &= QFile::link(m_paths[i], m_linkPaths[i]);
    }
    if (!result) {
    }
}

void LinkCommand::undo()
{
    bool result = true;
    for (int i = 0; i < m_paths.size(); i++) {
        result &= QFile::remove(m_linkPaths[i]);
    }
    if (!result) {
    }
}

void LinkCommand::appendPaths(const QString &sourcePath, const QString &linkPath)
{
    m_paths.append(sourcePath);
    m_linkPaths.append(linkPath);
}
