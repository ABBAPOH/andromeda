#ifndef FILESYSTEMMANAGER_H
#define FILESYSTEMMANAGER_H

#include "filemanagerplugin_global.h"

#include <QtCore/QObject>
#include <QtCore/QStringList>

class QUndoStack;
class QFileCopier;

namespace FileManagerPlugin {

class FileSystemManagerPrivate;
class FILEMANAGERPLUGIN_EXPORT FileSystemManager : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(FileSystemManager)
    Q_DISABLE_COPY(FileSystemManager)

public:
    enum FileOperationType {
        UnknownType = -1,
        Copy,
        Move,
        Link,
        Remove
    };
    Q_ENUMS(FileOperation)

    class FileOperation {
    public:
        enum State { Working, Done };

        FileOperation()
        {
            m_type = UnknownType;
            m_index = -1;
        }

        FileOperation(FileOperationType type, QStringList sources, QString dest, int index) :
            m_state(Working),
            m_type(type),
            m_sources(sources),
            m_dest(dest),
            m_index(index)
        {
        }

        FileOperation(const FileOperation &other) :
            m_type(other.m_type),
            m_sources(other.m_sources),
            m_dest(other.m_dest),
            m_destinationPaths(other.m_destinationPaths),
            m_index(other.m_index)
        {
        }

        inline State state() const { return m_state; }
        inline FileOperationType type() const { return m_type; }
        inline QStringList sources() const { return m_sources; }
        inline QString destination() const { return m_dest; }
        inline QStringList destinationPaths() const { return m_destinationPaths; }
        inline int index() const { return m_index; }

    protected:
        inline void setState(State state) { m_state = state; }

    private:
        State m_state;
        FileOperationType m_type;
        QStringList m_sources;
        QString m_dest;
        QStringList m_destinationPaths;
        int m_index;

        friend class FileSystemManager;
        friend class FileSystemManagerPrivate;
    };

    explicit FileSystemManager(QObject *parent = 0);
    ~FileSystemManager();

    int copy(const QStringList &files, const QString &destination);
    int move(const QStringList &files, const QString &destination);
    int link(const QStringList &files, const QString &destination);
    int remove(const QStringList &files);

    QList<FileOperation> operations() const;
    FileOperation currentOperation() const;
    int currentIndex() const;

    QFileCopier *copier(int index) const;

public slots:
    void redo();
    void undo();

signals:
    void canUndoChanged(bool);
    void canRedoChanged(bool);

    void started(int index);
    void finished(int index);

protected:
    FileSystemManagerPrivate *d_ptr;
};

} // namespace FileManagerPlugin

#endif // FILESYSTEMMANAGER_H
