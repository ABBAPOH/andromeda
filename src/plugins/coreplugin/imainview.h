#ifndef IMAINVIEW_H
#define IMAINVIEW_H

#include <QtCore/QString>
#include <iview.h>

#include "historyitem.h"

namespace CorePlugin {

class IMainView : public GuiSystem::IView
{
    Q_OBJECT
public:
    explicit IMainView(QObject *parent = 0);

    virtual bool open(const QString &path) = 0;
    virtual bool open(const HistoryItem &item) = 0;

    virtual HistoryItem currentItem() const = 0;

signals:

public slots:

};

} // namespace CorePlugin

#endif // IMAINVIEW_H
