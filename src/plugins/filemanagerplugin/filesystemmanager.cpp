#include "filesystemmanager.h"
#include "filesystemmanager_p.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QStringList>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtGui/QUndoCommand>

#include <QFileCopier>

using namespace FileManagerPlugin;

//QtFileCopier *FileSystemManagerPrivate::copier()
//{
//    QtFileCopier *copier = new QtFileCopier(q_ptr);
//    q_ptr->connect(copier, SIGNAL(done(bool)), copier, SLOT(deleteLater()));
//    emit q_ptr->operationStarted(copier);
//    return copier;
//}

QFileCopier *FileSystemManagerPrivate::copier(int index)
{
    QFileCopier *copier = new QFileCopier(this);
    copier->setAutoReset(false);
    registerCopier(copier, index);
    return copier;
}

void FileSystemManagerPrivate::registerCopier(QFileCopier *copier, int index)
{
    mapToCopier.insert(index, copier);
    connect(copier, SIGNAL(done(bool)), SLOT(onDone()));
    emit q_func()->started(index);
}

void FileSystemManagerPrivate::onDone()
{
    QFileCopier *copier = static_cast<QFileCopier *>(sender());
    int index = mapToCopier.key(copier);
    mapToCopier.remove(index);
    FileSystemManager::FileOperation &op = operations[index];
    QStringList dests;
    foreach (int i, copier->topRequests()) {
        dests.append(copier->destinationFilePath(i));
    }
    op.m_destinationPaths = dests;
    op.setState(FileSystemManager::FileOperation::Done);
    copier->deleteLater();
}

class FileSystemCommand : public QUndoCommand
{
public:
    explicit FileSystemCommand(QUndoCommand *parent = 0) : QUndoCommand(parent) {}

    FileSystemManager::FileOperation operation() const { return m_operation; }
    void setOperation(FileSystemManager::FileOperation operation) { m_operation = operation; }

    FileSystemManager *manager() const { return m_manager; }
    void setManager(FileSystemManager *manager) { m_manager = manager; }

    void setManagerPrivate(FileSystemManagerPrivate *manager) { m_managerPrivate = manager; }
    FileSystemManagerPrivate *managerPrivate() { return m_managerPrivate; }

private:
    FileSystemManagerPrivate *m_managerPrivate;
    FileSystemManager *m_manager;
    FileSystemManager::FileOperation m_operation;
};

//class CopyCommand : public FileSystemCommand
//{
//public:
//    explicit CopyCommand(QUndoCommand *parent = 0) : FileSystemCommand(parent) {}

//    void setDestination(const QString &path);
//    void setSourcePaths(const QStringList &paths);

//    virtual void redo();
//    virtual void undo();

//protected:
//    QString m_destinationPath;
//    QStringList m_sourcePaths;
//    QStringList m_destinationPaths;
//};

class CopyCommand2 : public FileSystemCommand
{
public:
    explicit CopyCommand2(QUndoCommand *parent = 0) : FileSystemCommand(parent) {}

    void redo();
    void undo();

protected:
    QStringList m_destinationPaths;
};

//class MoveCommand : public CopyCommand
//{
//public:
//    explicit MoveCommand(QUndoCommand *parent = 0) : CopyCommand(parent) {}

//    virtual void redo();
//    virtual void undo();
//protected:
//    QStringList m_destinationPaths;
//};

//class RenameCommand : public QUndoCommand
//{
//public:
//    explicit RenameCommand(QUndoCommand *parent = 0) : QUndoCommand(parent) {}

//    void setPath(const QString &path);
//    void setNewName(const QString &name);

//    virtual void redo();
//    virtual void undo();

//protected:
//    QString m_path;
//    QString m_newName;
//};

//class LinkCommand : public QUndoCommand
//{
//public:
//    explicit LinkCommand(QUndoCommand *parent = 0) : QUndoCommand(parent) {}

//    void appendPaths(const QString &sourcePath, const QString &linkPath);

//    virtual void redo();
//    virtual void undo();

//protected:
//    QStringList m_paths;
//    QStringList m_linkPaths;
//};

/* ================================ Implementation ================================ */

//void CopyCommand::setDestination(const QString &path)
//{
//    m_destinationPath = path;
//}

//void CopyCommand::setSourcePaths(const QStringList &paths)
//{
//    m_sourcePaths = paths;
//}

//void CopyCommand::redo()
//{
//    QtFileCopier * copier = managerPrivate()->copier();

//    m_destinationPaths.clear();
//    foreach (const QString &filePath, m_sourcePaths) {
//        QFileInfo info(filePath);
//        QString targetPath = m_destinationPath;
//        QDir dir(m_destinationPath);

//        // TODO: give different names when copying to same dir
//        if (dir == info.dir()) {
//            int i = 0;
//            do  {
//                i++;
//                targetPath = dir.absolutePath() + QDir::separator() + info.completeBaseName() + ' ' +
//                        QString::number(i);
//                if (info.suffix() != "") {
//                    targetPath += '.' + info.suffix();
//                }
//            } while (QFileInfo(targetPath).exists());
//            m_destinationPaths.append(targetPath);
//        } else {
//            m_destinationPaths.append(targetPath + QDir::separator() + info.fileName());
//        }

//        if (info.isDir()) {
//            copier->copyDirectory(filePath, targetPath);
//        } else {
//            copier->copy(filePath, targetPath);
//        }
//    }
//}

//bool removePath(const QString &path)
//{
//    bool result = true;
//    QFileInfo info(path);
//    if (info.isDir()) {
//        QDir dir(path);
//        foreach (const QString &entry, dir.entryList(QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot)) {
//            result &= removePath(dir.absoluteFilePath(entry));
//        }
//        if (!info.dir().rmdir(info.fileName()))
//            return false;
//    } else {
//        result = QFile::remove(path);
//    }
//    return result;
//}

//void CopyCommand::undo()
//{
//    foreach (const QString &path, m_destinationPaths) {
//        removePath(path);
//    }
//}

void CopyCommand2::redo()
{
    const FileSystemManager::FileOperation &op = operation();

    QFileCopier *copier = managerPrivate()->copier(op.index());
    copier->copy(op.sources(), op.destination());
}

void CopyCommand2::undo()
{
    const FileSystemManager::FileOperation &op = operation();

    QFileCopier *copier = managerPrivate()->copier(op.index());
    copier->remove(op.destinationPaths());
}

//void MoveCommand::redo()
//{
//    QtFileCopier * copier = managerPrivate()->copier();

//    m_destinationPaths.clear();
//    foreach (const QString &filePath, m_sourcePaths) {
//        QFileInfo info(filePath);
//        QString targetPath = m_destinationPath;
//        QDir dir(m_destinationPath);

//        // TODO: give different names when copying to same dir
//        if (dir == info.dir()) {
//            int i = 0;
//            do  {
//                i++;
//#ifndef Q_CC_MSVC
//#warning "Uses mimetypes to determine type and extension"
//#endif
//                targetPath = dir.absolutePath() + QDir::separator() + info.completeBaseName() + ' ' +
//                        QString::number(i);
//                if (info.suffix() != "") {
//                    targetPath += '.' + info.suffix();
//                }
//            } while (QFileInfo(targetPath).exists());
//            m_destinationPaths.append(targetPath);
//        } else {
//            m_destinationPaths.append(targetPath + QDir::separator() + info.fileName());
//        }

//        if (info.isDir()) {
//            copier->moveDirectory(filePath, targetPath);
//        } else {
//            copier->move(filePath, targetPath);
//        }
//    }
//}

//void MoveCommand::undo()
//{
//    QtFileCopier * copier = managerPrivate()->copier();

//    int i = 0;
//    foreach (const QString &filePath, m_destinationPaths) {
//        QFileInfo info(filePath);
//        QFileInfo sourceInfo(m_sourcePaths[i]);
//        QDir dir = sourceInfo.dir();

//        // TODO: give different names when copying to same dir
//        if (info.isDir()) {
//            copier->moveDirectory(filePath, dir.absoluteFilePath(info.fileName()));
//        } else {
//            copier->move(filePath, dir.absoluteFilePath(info.fileName()));
//        }
//        i++;
//    }
//}

//void RenameCommand::setPath(const QString &path)
//{
//    m_path = path;
//}

//void RenameCommand::setNewName(const QString &name)
//{
//    m_newName = name;
//}

//void RenameCommand::redo()
//{
//    QFile(m_path).rename(m_newName);
//}

//void RenameCommand::undo()
//{
//    QFileInfo info(m_path);
//    QString newPath = info.dir().absoluteFilePath(m_newName);
//    QFile(newPath).rename(info.fileName());
//}

//void LinkCommand::redo()
//{
//    bool result = true;
//    for (int i = 0; i < m_paths.size(); i++) {
//        result &= QFile::link(m_paths[i], m_linkPaths[i]);
//    }
//    if (!result) {
//    }
//}

//void LinkCommand::undo()
//{
//    bool result = true;
//    for (int i = 0; i < m_paths.size(); i++) {
//        result &= QFile::remove(m_linkPaths[i]);
//    }
//    if (!result) {
//    }
//}

//void LinkCommand::appendPaths(const QString &sourcePath, const QString &linkPath)
//{
//    m_paths.append(sourcePath);
//    m_linkPaths.append(linkPath);
//}

/* ================================ FileSystemManager ================================ */

FileSystemManager::FileSystemManager(QObject *parent) :
    QObject(parent),
    d_ptr(new FileSystemManagerPrivate(this))
{
    Q_D(FileSystemManager);

    d->undoStack = new QUndoStack(this);
    d->currentIndex = -1;

    connect(d->undoStack, SIGNAL(canRedoChanged(bool)), SIGNAL(canRedoChanged(bool)));
    connect(d->undoStack, SIGNAL(canUndoChanged(bool)), SIGNAL(canUndoChanged(bool)));
}

FileSystemManager::~FileSystemManager()
{
    delete d_ptr;
}

int FileSystemManager::copy(const QStringList &files, const QString &destination)
{
    Q_D(FileSystemManager);

    int index = d->currentIndex + 1;
    FileOperation op(Copy, files, destination, index);
    op.setState(FileOperation::Working);

    d->operations.erase(d->operations.begin() + index, d->operations.end());
    d->operations.append(op);

    CopyCommand2 *cmd = new CopyCommand2;
    cmd->setManager(this);
    cmd->setManagerPrivate(d);
    cmd->setOperation(op);
    d->undoStack->push(cmd);

    return index;
}

int FileSystemManager::move(const QStringList &files, const QString &destination)
{
    return -1;
}

QList<FileSystemManager::FileOperation> FileSystemManager::operations() const
{
    return d_func()->operations;
}

FileSystemManager::FileOperation FileSystemManager::currentOperation() const
{
    Q_D(const FileSystemManager);

    return d->operations.at(d->currentIndex);
}

int FileSystemManager::currentIndex() const
{
    return d_func()->currentIndex;
}

QFileCopier * FileSystemManager::copier(int index) const
{
    return d_func()->mapToCopier.value(index);
}

void FileSystemManager::redo()
{
    Q_D(FileSystemManager);

    d->undoStack->redo();

    int index = d->undoStack->index();
    const FileSystemCommand *cmd = static_cast<const FileSystemCommand *>(d->undoStack->command(index));

    d->currentIndex = cmd->operation().index();
}

void FileSystemManager::undo()
{
    Q_D(FileSystemManager);

    d->undoStack->undo();

    int index = d->undoStack->index();
    const FileSystemCommand *cmd = static_cast<const FileSystemCommand *>(d->undoStack->command(index));

    d->currentIndex = cmd->operation().index();
}
