#ifndef FILEMANAGEREDITOR_P_H
#define FILEMANAGEREDITOR_P_H

#include "filemanagereditor.h"
#include "dualpanewidget.h"
#include <guisystem/historyitem.h>
#include <guisystem/ihistory.h>

namespace FileManager {

class FileManagerHistory : public GuiSystem::IHistory
{
    Q_OBJECT
    Q_DISABLE_COPY(FileManagerHistory)

public:
    explicit FileManagerHistory(QObject *parent = 0);

    inline void setDualPaneWidget(DualPaneWidget *widget) { m_widget = widget; }

    virtual void clear();
    virtual int count() const;

    virtual int currentItemIndex() const;
    virtual void setCurrentItemIndex(int index);

    virtual GuiSystem::HistoryItem itemAt(int index) const;

private:
    DualPaneWidget *m_widget;
};

} //namespace FileManager

#endif // FILEMANAGEREDITOR_P_H
