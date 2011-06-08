#ifndef PAGE_H
#define PAGE_H

#include "coreplugin_global.h"

#include <QtCore/QObject>
#include <state.h>

#include "history.h"

namespace CorePlugin {

class IMainView;
class COREPLUGIN_EXPORT Page : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString currentPath READ currentPath WRITE setCurrentPath NOTIFY currentPathChanged)
public:
    explicit Page(QObject *parent = 0);

    QString currentPath() const;

    History *history() const;

    GuiSystem::State *state() const;
    void setState(GuiSystem::State *state);

public slots:
    void setCurrentPath(QString currentPath);

signals:
    void currentPathChanged(QString currentPath);

private slots:
    void onIndexChanged(int index);

private:
    IMainView *getMainView(const QString &perspective);
    QString getPerspective(const QString &path);
    void openPerspective(const QString &path);
    void openPerspective(const HistoryItem &item);

    QString m_currentPath;
    History *m_history;
};

} // namespace CorePlugin

#endif // PAGE_H
