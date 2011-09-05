#include "filesystemmanager.h"
#include "filesystemmanager_p.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QStringList>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtGui/QUndoCommand>

#include <QFileCopier>

using namespace FileManagerPlugin;

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

int FileSystemManagerPrivate::newOperation(FileSystemManager::FileOperationType type, const QStringList &files, const QString &destination)
{
    currentIndex = currentIndex + 1;
    FileSystemManager::FileOperation op(type, files, destination, currentIndex);
    op.setState(FileSystemManager::FileOperation::Working);

    operations.erase(operations.begin() + currentIndex, operations.end());
    operations.append(op);
    return currentIndex;
}

void FileSystemManagerPrivate::onDone()
{
    QFileCopier *copier = static_cast<QFileCopier *>(sender());
    int index = mapToCopier.key(copier);
    mapToCopier.remove(index);
    copier->deleteLater();

    FileSystemManager::FileOperation &op = operations[index];
    op.setState(FileSystemManager::FileOperation::Done);
    if (op.isUndo()) {
        setOperationUndo(op, false);
        return;
    }

    QStringList dests;
    foreach (int i, copier->topRequests()) {
        dests.append(copier->destinationFilePath(i));
    }
    op.m_destinationPaths = dests;
}

class FileSystemCommand : public QUndoCommand
{
public:
    explicit FileSystemCommand(QUndoCommand *parent = 0) : QUndoCommand(parent) {}

    int operationIndex() const { return m_operationIndex; }
    void setOperationIndex(int operationIndex) { m_operationIndex = operationIndex; }

    FileSystemManager *manager() const { return m_manager; }
    void setManager(FileSystemManager *manager) { m_manager = manager; }

    void setManagerPrivate(FileSystemManagerPrivate *manager) { m_managerPrivate = manager; }
    FileSystemManagerPrivate *managerPrivate() { return m_managerPrivate; }

private:
    FileSystemManagerPrivate *m_managerPrivate;
    FileSystemManager *m_manager;
    int m_operationIndex;
};

class CopyCommand : public FileSystemCommand
{
public:
    explicit CopyCommand(QUndoCommand *parent = 0) : FileSystemCommand(parent) {}

    void redo();
    void undo();
};

class MoveCommand : public FileSystemCommand
{
public:
    explicit MoveCommand(QUndoCommand *parent = 0) : FileSystemCommand(parent) {}

    virtual void redo();
    virtual void undo();
};

class LinkCommand : public FileSystemCommand
{
public:
    explicit LinkCommand(QUndoCommand *parent = 0) : FileSystemCommand(parent) {}

    void redo();
    void undo();
};

void CopyCommand::redo()
{
    int index = operationIndex();

    const FileSystemManager::FileOperation &op = managerPrivate()->operations[index];

    QFileCopier *copier = managerPrivate()->copier(op.index());
    copier->copy(op.sources(), op.destination());
}

void CopyCommand::undo()
{
    int index = operationIndex();
    FileSystemManager::FileOperation &op = managerPrivate()->operations[index];
    FileSystemManagerPrivate::setOperationUndo(op, true);

    QFileCopier *copier = managerPrivate()->copier(op.index());
    copier->remove(op.destinationPaths());
}

void MoveCommand::redo()
{
    int index = operationIndex();
    const FileSystemManager::FileOperation &op = managerPrivate()->operations[index];

    QFileCopier *copier = managerPrivate()->copier(op.index());
    copier->move(op.sources(), op.destination(), QFileCopier::CopyOnMove);
}

void MoveCommand::undo()
{
    int index = operationIndex();
    const FileSystemManager::FileOperation &op = managerPrivate()->operations[index];

    QFileCopier *copier = managerPrivate()->copier(op.index());
    for (int i = 0; i < op.destinationPaths().size(); i++) {
        copier->move(op.destinationPaths()[i], op.sources()[i], QFileCopier::CopyOnMove);
    }
}

void LinkCommand::redo()
{
    int index = operationIndex();
    const FileSystemManager::FileOperation &op = managerPrivate()->operations[index];

    QFileCopier *copier = managerPrivate()->copier(op.index());
    copier->link(op.sources(), op.destination());
}

void LinkCommand::undo()
{
    int index = operationIndex();
    const FileSystemManager::FileOperation &op = managerPrivate()->operations[index];

    QFileCopier *copier = managerPrivate()->copier(op.index());
    copier->remove(op.destinationPaths());
}

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

    int index = d->newOperation(Copy, files, destination);

    CopyCommand *cmd = new CopyCommand;
    cmd->setManager(this);
    cmd->setManagerPrivate(d);
    cmd->setOperationIndex(index);
    d->undoStack->push(cmd);

    return index;
}

int FileSystemManager::move(const QStringList &files, const QString &destination)
{
    Q_D(FileSystemManager);

    int index = d->newOperation(Move, files, destination);

    MoveCommand *cmd = new MoveCommand;
    cmd->setManager(this);
    cmd->setManagerPrivate(d);
    cmd->setOperationIndex(index);
    d->undoStack->push(cmd);

    return index;
}

int FileSystemManager::link(const QStringList &files, const QString &destination)
{
    Q_D(FileSystemManager);

    int index = d->newOperation(Link, files, destination);

    LinkCommand *cmd = new LinkCommand;
    cmd->setManager(this);
    cmd->setManagerPrivate(d);
    cmd->setOperationIndex(index);
    d->undoStack->push(cmd);

    return index;
}

int FileSystemManager::remove(const QStringList &files)
{
    Q_D(FileSystemManager);

    int index = d->newOperation(Remove, files, QString());

    QFileCopier *copier = d->copier(index);
    copier->remove(files);
    return index;
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

    int index = d->undoStack->index() - 1;
    const FileSystemCommand *cmd = static_cast<const FileSystemCommand *>(d->undoStack->command(index));

    d->currentIndex = cmd->operationIndex();
}

void FileSystemManager::undo()
{
    Q_D(FileSystemManager);

    d->undoStack->undo();

    int index = d->undoStack->index();
    const FileSystemCommand *cmd = static_cast<const FileSystemCommand *>(d->undoStack->command(index));

    d->currentIndex = cmd->operationIndex();
}
