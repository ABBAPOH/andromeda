#include "filesystemmanager.h"
#include "filesystemmanager_p.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QStringList>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtGui/QUndoCommand>

#include <io/QDriveInfo>
#include <io/QFileCopier>
#include <io/qtrash.h>

using namespace FileManager;

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
    Q_Q(FileSystemManager);

    QFileCopier *copier = static_cast<QFileCopier *>(sender());
    int index = mapToCopier.key(copier);
    mapToCopier.remove(index);
    copier->deleteLater();

    FileSystemManager::FileOperation &op = operations[index];
    op.setState(FileSystemManager::FileOperation::Done);
    if (op.isUndo()) {
        setOperationUndo(op, false);
        if (!canRedo) {
            canRedo = true;
            emit q->canRedoChanged(true);
        }
        return;
    }

    QStringList dests;
    foreach (int i, copier->topRequests()) {
        dests.append(copier->destinationFilePath(i));
    }
    op.m_destinationPaths = dests;

    if (!op.isUndo()) {
//        bool b = q->canUndoChanged();
        if (!canUndo) {
            canUndo = true;
            emit q->canUndoChanged(true);
        }
    }
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

class MoveToTrashCommand : public FileSystemCommand
{
public:
    explicit MoveToTrashCommand(QUndoCommand *parent = 0) : FileSystemCommand(parent) {}

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

static QString getExistingPath(const QString &path)
{
    QString dir = path;
    while (!QFile::exists(dir) && !dir.isEmpty()) {
        dir = QFileInfo(dir).absolutePath();
    }
    return dir;
}

static bool canMove(const QStringList &sources, const QString &dest)
{
    QDriveInfo destDrive(getExistingPath(dest));

    foreach (const QString &source, sources) {
        QDriveInfo sourceDrive(getExistingPath(source));
        if (destDrive != sourceDrive)
            return false;
    }

    return true;
}

void MoveCommand::redo()
{
    int index = operationIndex();
    const FileSystemManager::FileOperation &op = managerPrivate()->operations[index];

    QFileCopier *copier = managerPrivate()->copier(op.index());
    bool canMove = ::canMove(op.sources(), op.destination());
    copier->move(op.sources(), op.destination(), canMove ? QFileCopier::CopyFlag(0) : QFileCopier::CopyOnMove);
}

void MoveCommand::undo()
{
    int index = operationIndex();
    const FileSystemManager::FileOperation &op = managerPrivate()->operations[index];

    QFileCopier *copier = managerPrivate()->copier(op.index());
    for (int i = 0; i < op.destinationPaths().size(); i++) {
        bool canMove = ::canMove(QStringList() << op.destinationPaths()[i], op.sources()[i]);
        QFileCopier::CopyFlags flags = canMove ? QFileCopier::CopyFlag(0) : QFileCopier::CopyOnMove;
        copier->move(op.destinationPaths()[i], op.sources()[i], flags);
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

void MoveToTrashCommand::redo()
{
    int index = operationIndex();
    FileSystemManager::FileOperation &op = managerPrivate()->operations[index];

    QTrash t;
    op.m_destinationPaths.clear();
    foreach (const QString &source, op.sources()) {
        QString dest;
        t.moveToTrash(source, &dest);
        op.m_destinationPaths.append(dest);
    }

    op.setState(FileSystemManager::FileOperation::Done);
    managerPrivate()->canUndo = true;
    QMetaObject::invokeMethod(manager(), "canUndoChanged", Q_ARG(bool, true));
}

void MoveToTrashCommand::undo()
{
    int index = operationIndex();
    FileSystemManager::FileOperation &op = managerPrivate()->operations[index];

    QTrash t;
    foreach (const QString &dest, op.destinationPaths()) {
        t.restore(dest);
    }

    op.setState(FileSystemManager::FileOperation::Done);
    managerPrivate()->canRedo = true;
    QMetaObject::invokeMethod(manager(), "canRedoChanged", Q_ARG(bool, true));
}

FileSystemManager::FileSystemManager(QObject *parent) :
    QObject(parent),
    d_ptr(new FileSystemManagerPrivate(this))
{
    Q_D(FileSystemManager);

    d->undoStack = new QUndoStack(this);
    d->currentIndex = -1;
    d->canUndo = false;
    d->canRedo = false;

//    connect(d->undoStack, SIGNAL(canRedoChanged(bool)), SIGNAL(canRedoChanged(bool)));
//    connect(d->undoStack, SIGNAL(canUndoChanged(bool)), SIGNAL(canUndoChanged(bool)));
}

FileSystemManager::~FileSystemManager()
{
    delete d_ptr;
}

Q_GLOBAL_STATIC(FileSystemManager, staticManager)
FileSystemManager *FileSystemManager::instance()
{
    return staticManager();
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

int FileSystemManager::moveToTrash(const QStringList &files)
{
    Q_D(FileSystemManager);

    int index = d->newOperation(MoveToTrash, files, QString());

    MoveToTrashCommand *cmd = new MoveToTrashCommand;
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

bool FileSystemManager::canRedo() const
{
    Q_D(const FileSystemManager);

    return d->canRedo && d->undoStack->canRedo();
}

bool FileSystemManager::canUndo() const
{
    Q_D(const FileSystemManager);

    return d->canUndo && d->undoStack->canUndo();
}

void FileSystemManager::redo()
{
    Q_D(FileSystemManager);

    d->undoStack->redo();

    int index = d->undoStack->index() - 1;
    const FileSystemCommand *cmd = static_cast<const FileSystemCommand *>(d->undoStack->command(index));

    d->currentIndex = cmd->operationIndex();

    if (!d->undoStack->canRedo()) {
        d->canRedo = false;
        emit canRedoChanged(false);
    }
}

void FileSystemManager::undo()
{
    Q_D(FileSystemManager);

    d->undoStack->undo();

    int index = d->undoStack->index();
    const FileSystemCommand *cmd = static_cast<const FileSystemCommand *>(d->undoStack->command(index));

    d->currentIndex = cmd->operationIndex();

    if (!d->undoStack->canUndo()) {
        d->canUndo = false;
        emit canUndoChanged(false);
    }
}
