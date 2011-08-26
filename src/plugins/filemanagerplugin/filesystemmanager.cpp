#include "filesystemmanager.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QStringList>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtGui/QUndoCommand>

#include "QtFileCopier"

namespace FileManagerPlugin {

class FileSystemManagerPrivate
{
public:
    FileSystemManagerPrivate(FileSystemManager *qq) : q_ptr(qq) {}

    QtFileCopier *copier();

    QUndoStack *undoStack;
    FileSystemManager *q_ptr;
};

} // namespace FileManagerPlugin

using namespace FileManagerPlugin;

QtFileCopier *FileSystemManagerPrivate::copier()
{
    QtFileCopier *copier = new QtFileCopier(q_ptr);
    q_ptr->connect(copier, SIGNAL(done(bool)), copier, SLOT(deleteLater()));
    emit q_ptr->operationStarted(copier);
    return copier;
}

class FileSystemCommand : public QUndoCommand
{
public:
    explicit FileSystemCommand(QUndoCommand *parent = 0) : QUndoCommand(parent) {}

    void setManager(FileSystemManagerPrivate *manager) { m_manager = manager; }
    FileSystemManagerPrivate *manager() { return m_manager; }

private:
    FileSystemManagerPrivate *m_manager;
};

class CopyCommand : public FileSystemCommand
{
public:
    explicit CopyCommand(QUndoCommand *parent = 0) : FileSystemCommand(parent) {}

    void setDestination(const QString &path);
    void setSourcePaths(const QStringList &paths);

    virtual void redo();
    virtual void undo();

protected:
    QString m_destinationPath;
    QStringList m_sourcePaths;
    QStringList m_destinationPaths;
};

class MoveCommand : public CopyCommand
{
public:
    explicit MoveCommand(QUndoCommand *parent = 0) : CopyCommand(parent) {}

    virtual void redo();
    virtual void undo();
protected:
    QStringList m_destinationPaths;
};

class RenameCommand : public QUndoCommand
{
public:
    explicit RenameCommand(QUndoCommand *parent = 0) : QUndoCommand(parent) {}

    void setPath(const QString &path);
    void setNewName(const QString &name);

    virtual void redo();
    virtual void undo();

protected:
    QString m_path;
    QString m_newName;
};

class LinkCommand : public QUndoCommand
{
public:
    explicit LinkCommand(QUndoCommand *parent = 0) : QUndoCommand(parent) {}

    void appendPaths(const QString &sourcePath, const QString &linkPath);

    virtual void redo();
    virtual void undo();

protected:
    QStringList m_paths;
    QStringList m_linkPaths;
};

/* ================================ Implementation ================================ */

void CopyCommand::setDestination(const QString &path)
{
    m_destinationPath = path;
}

void CopyCommand::setSourcePaths(const QStringList &paths)
{
    m_sourcePaths = paths;
}

void CopyCommand::redo()
{
    QtFileCopier * copier = manager()->copier();

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
        QDir dir(path);
        foreach (const QString &entry, dir.entryList(QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot)) {
            result &= removePath(dir.absoluteFilePath(entry));
        }
        if (!info.dir().rmdir(info.fileName()))
            return false;
    } else {
        result = QFile::remove(path);
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
    QtFileCopier * copier = manager()->copier();

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
#ifndef Q_CC_MSVC
#warning "Uses mimetypes to determine type and extension"
#endif
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
    QtFileCopier * copier = manager()->copier();

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

/* ================================ FileSystemManager ================================ */

FileSystemManager::FileSystemManager(QObject *parent) :
    QObject(parent),
    d_ptr(new FileSystemManagerPrivate(this))
{
    Q_D(FileSystemManager);
    d->undoStack = new QUndoStack(this);
}

FileSystemManager::~FileSystemManager()
{
    delete d_ptr;
}

QUndoStack *FileSystemManager::undoStack() const
{
    return d_ptr->undoStack;
}

void FileSystemManager::copyFiles(const QStringList &files, const QString &destination)
{
    CopyCommand *command = new CopyCommand;
    command->setManager(d_func());
    command->setSourcePaths(files);
    command->setDestination(destination);
    undoStack()->push(command);
}

void FileSystemManager::moveFiles(const QStringList &files, const QString &destination)
{
    MoveCommand *command = new MoveCommand;
    command->setManager(d_func());
    command->setSourcePaths(files);
    command->setDestination(destination);
    undoStack()->push(command);
}
