#ifndef FILEMANAGEREDITOR_P_H
#define FILEMANAGEREDITOR_P_H

#include "filemanagereditor.h"

#include <Parts/HistoryItem>
#include <Parts/IHistory>

namespace FileManager {

class FileManagerEditorHistory : public Parts::IHistory
{
    Q_OBJECT
    Q_DISABLE_COPY(FileManagerEditorHistory)

public:
    explicit FileManagerEditorHistory(QObject *parent = 0);

    void setHistory(FileManagerHistory *history);

    void clear();
    void erase();
    int count() const;

    int currentItemIndex() const;
    void setCurrentItemIndex(int index);

    Parts::HistoryItem itemAt(int index) const;

    QByteArray store() const;
    void restore(const QByteArray &history);

private:
    FileManagerHistory *m_history;
};

} //namespace FileManager

#endif // FILEMANAGEREDITOR_P_H
