#ifndef FILESYSTEMUNDOMANAGER_H
#define FILESYSTEMUNDOMANAGER_H

#include <QtCore/QObject>
#include <QtCore/QStringList>
#include <QtGui/QUndoCommand>

class QUndoStack;

namespace FileManagerPlugin {

class FileSystemUndoManager : public QObject
{
    Q_OBJECT
public:
    explicit FileSystemUndoManager(QObject *parent = 0);

    QUndoStack *undoStack() const;

signals:

public slots:
};

class CopyCommand : public QUndoCommand
{
public:
    explicit CopyCommand(QUndoCommand *parent = 0) : QUndoCommand(parent) {}

    void setDestination(const QString &path);
    void setSourcePaths(const QStringList &paths);
    void appendSourcePath(const QString &path);

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

} // namespace FileManagerPlugin

#endif // FILESYSTEMUNDOMANAGER_H
