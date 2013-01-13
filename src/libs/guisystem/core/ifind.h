#ifndef IFIND_H
#define IFIND_H

#include "../guisystem_global.h"

#include <QtCore/QObject>

namespace GuiSystem {

class GUISYSTEM_EXPORT IFind : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(IFind)

public:
    enum FindFlag {
        FindBackward = 0x01,
        FindCaseSensitively = 0x02,
        FindWholeWords = 0x04,
        FindRegularExpression = 0x08
    };
    Q_DECLARE_FLAGS(FindFlags, FindFlag)

    explicit IFind(QObject *parent = 0);
    ~IFind();

    virtual bool supportsReplace() const;
    virtual FindFlags supportedFindFlags() const = 0;

    virtual void resetIncrementalSearch();
    virtual void clearResults() = 0;

    virtual QString currentFindString() const = 0;
    virtual QString completedFindString() const = 0;

    virtual void highlightAll(const QString &txt, FindFlags findFlags);
    virtual void findIncremental(const QString &txt, FindFlags findFlags) = 0;
    virtual void findStep(const QString &txt, FindFlags findFlags) = 0;
    virtual void replace(const QString &before, const QString &after, FindFlags findFlags);
    virtual bool replaceStep(const QString &before, const QString &after, FindFlags findFlags);
    virtual int replaceAll(const QString &before, const QString &after, FindFlags findFlags);

signals:
    void finished(bool found);
//    void currentChanged(int entry);
    void countChanged(int entries);
};

} // namespace GuiSystem

#endif // IFIND_H
