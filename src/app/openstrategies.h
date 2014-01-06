#ifndef OPENSTRATEGIES_H
#define OPENSTRATEGIES_H

#include <Parts/OpenStrategy>

namespace Andromeda {

class OpenCurrentWindowStrategy : public Parts::OpenStrategy
{
    Q_OBJECT
    Q_DISABLE_COPY(OpenCurrentWindowStrategy)
public:
    explicit OpenCurrentWindowStrategy(QObject *parent = 0);

    bool canOpen(const QList<QUrl> &urls);
    void open(const QList<QUrl> &urls);
    QString text() const;
};

class OpenNewTabStrategy : public Parts::OpenStrategy
{
    Q_OBJECT
    Q_DISABLE_COPY(OpenNewTabStrategy)
public:
    explicit OpenNewTabStrategy(QObject *parent = 0);

    bool canOpen(const QList<QUrl> &urls);
    void open(const QList<QUrl> &urls);
    QString text() const;
    Qt::KeyboardModifiers modifiers() const;
};

class OpenNewWindowStrategy : public Parts::OpenStrategy
{
    Q_OBJECT
    Q_DISABLE_COPY(OpenNewWindowStrategy)
public:
    explicit OpenNewWindowStrategy(QObject *parent = 0);

    bool canOpen(const QList<QUrl> &urls);
    void open(const QList<QUrl> &urls);
    QString text() const;
    Qt::KeyboardModifiers modifiers() const;
};

class OpenEditorStrategy : public Parts::OpenStrategy
{
    Q_OBJECT
    Q_DISABLE_COPY(OpenEditorStrategy)
public:
    explicit OpenEditorStrategy(QObject *parent = 0);

    bool canOpen(const QList<QUrl> &urls);
    void open(const QList<QUrl> &urls);
    QString text() const;
    QKeySequence keySequence() const;
};

} // namespace Andromeda

#endif // OPENSTRATEGIES_H
