#include "filesystemundomanager.h"

#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtGui/QApplication>
#include <QtGui/QUndoStack>
#include <IO>

#include <QDebug>

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

    m_destinationPaths.clear();
    foreach (const QString &filePath, m_sourcePaths) {
        QFileInfo info(filePath);
        QString targetPath = m_destinationPath;
        QDir dir(m_destinationPath);

        // TODO: give different names when copying to same dir
        if (dir == info.dir()) {
            int i = 0;
            do  {
                i++;
                targetPath = dir.absolutePath() + QDir::separator() + info.completeBaseName() + ' ' +
                        QString::number(i);
                if (info.suffix() != "") {
                    targetPath += '.' + info.suffix();
                }
            } while (QFileInfo(targetPath).exists());
            m_destinationPaths.append(targetPath);
        } else {
            m_destinationPaths.append(targetPath + QDir::separator() + info.fileName());
        }

        if (info.isDir()) {
            copier->copyDirectory(filePath, targetPath);
        } else {
            copier->copy(filePath, targetPath);
        }
    }
}

bool removePath(const QString &path)
{
    bool result = true;
    QFileInfo info(path);
    if (info.isDir()) {
        foreach (const QString &path, QDir(path).entryList(QStringList(), QDir::NoDotAndDotDot)) {
            result &= removePath(path);
        }
        info.dir().rmdir(info.fileName());
    } else {
        result &= QFile::remove(path);
    }
    return result;
}

void CopyCommand::undo()
{
    foreach (const QString &path, m_destinationPaths) {
        removePath(path);
    }
}

void MoveCommand::redo()
{
    FileCopyManager * manager = FileCopyManager::instance();
    QtFileCopier * copier = manager->copier();

    m_destinationPaths.clear();
    foreach (const QString &filePath, m_sourcePaths) {
        QFileInfo info(filePath);
        QString targetPath = m_destinationPath;
        QDir dir(m_destinationPath);

        // TODO: give different names when copying to same dir
        if (dir == info.dir()) {
            int i = 0;
            do  {
                i++;
                targetPath = dir.absolutePath() + QDir::separator() + info.completeBaseName() + ' ' +
                        QString::number(i);
                if (info.suffix() != "") {
                    targetPath += '.' + info.suffix();
                }
            } while (QFileInfo(targetPath).exists());
            m_destinationPaths.append(targetPath);
        } else {
            m_destinationPaths.append(targetPath + QDir::separator() + info.fileName());
        }

        if (info.isDir()) {
            copier->moveDirectory(filePath, targetPath);
        } else {
            copier->move(filePath, targetPath);
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
