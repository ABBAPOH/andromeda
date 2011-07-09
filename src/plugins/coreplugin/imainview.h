#ifndef IMAINVIEW_H
#define IMAINVIEW_H

#include <QtCore/QString>
#include <iview.h>

#include "historyitem.h"

namespace CorePlugin {

class COREPLUGIN_EXPORT IMainView : public GuiSystem::IView
{
    Q_OBJECT
public:
    explicit IMainView(QObject *parent = 0);

    virtual bool open(const QString &path) = 0;
    virtual bool open(const HistoryItem &item) = 0;

    virtual HistoryItem currentItem() const = 0;

signals:
    void openRequested(const QString &path);
    void pathChanged(const QString &);
    void currentItemChanged();

private slots:
    void onPathChanged(const QString &);
};

} // namespace CorePlugin

#endif // IMAINVIEW_H
