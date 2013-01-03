#ifndef FILEMANAGERHISTORY_P_H
#define FILEMANAGERHISTORY_P_H

#include "filemanagerhistory.h"

namespace FileManager {

class FileManagerHistoryPrivate
{
public:
    FileManagerHistoryPrivate(FileManagerHistory *qq) : q(qq) {}

    QList<FileManagerHistoryItem> items;
    int maximumItemCount;
    int currentItemIndex;

    FileManagerHistory *q;

    void appendItem(const FileManagerHistoryItem &item);
    void setCurrentItemIndex(int index);
};

} // namespace FileManager

using namespace FileManager;

#endif // FILEMANAGERHISTORY_P_H
