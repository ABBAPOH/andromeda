#ifndef FILEMANAGERHISTORYITEM_P_H
#define FILEMANAGERHISTORYITEM_P_H

#include "filemanagerhistoryitem.h"

namespace FileManager {

class FileManagerHistoryItemData : public QSharedData
{
public:
    QIcon icon;
    bool valid;
    QDateTime lastVisited;
    QString title;
    QString path;
};

} // namespace FileManager

#endif // FILEMANAGERHISTORYITEM_P_H
