#ifndef FILEMANAGEREDITOR_P_H
#define FILEMANAGEREDITOR_P_H

#include "filemanagereditor.h"

#include <GuiSystem/HistoryItem>
#include <GuiSystem/IHistory>

namespace FileManager {

class FileManagerEditorHistory : public GuiSystem::IHistory
{
    Q_OBJECT
    Q_DISABLE_COPY(FileManagerEditorHistory)

public:
    explicit FileManagerEditorHistory(QObject *parent = 0);

    void setDualPaneWidget(FileManagerWidget *widget);

    void clear();
    void erase();
    int count() const;

    int currentItemIndex() const;
    void setCurrentItemIndex(int index);

    GuiSystem::HistoryItem itemAt(int index) const;

    QByteArray store() const;
    void restore(const QByteArray &history);

private:
    FileManagerWidget *m_widget;
};

} //namespace FileManager

#endif // FILEMANAGEREDITOR_P_H
