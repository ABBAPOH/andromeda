#include "globalhistory.h"

namespace CorePlugin {

class GlobalHistoryPrivate
{
public:
    QList<GlobalHistoryItem *> items;
};

} // namespace CorePlugin

using namespace CorePlugin;

GlobalHistory::GlobalHistory(QObject *parent) :
    QObject(parent),
    d_ptr(new GlobalHistoryPrivate)
{
    GlobalHistoryItem item;
    item.type = GlobalHistoryItem::View;
    item.index = 10;
}

GlobalHistory::~GlobalHistory()
{
    delete d_ptr;
}

void GlobalHistory::back()
{
}

void GlobalHistory::forward()
{
}
