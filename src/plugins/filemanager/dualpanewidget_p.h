#ifndef DUALPANEWIDGET_P_H
#define DUALPANEWIDGET_P_H

#include "dualpanewidget.h"

class QVBoxLayout;
class QSplitter;
class QActionGroup;

namespace FileManager {

class DualPaneWidgetPrivate : public QObject
{
    Q_OBJECT
    Q_DECLARE_PUBLIC(DualPaneWidget)

public:
    explicit DualPaneWidgetPrivate(DualPaneWidget *qq) : q_ptr(qq) {}

    void createActions();
    void retranslateUi();
    FileManagerWidget *createPane();
    void createLeftPane();
    void createRightPane();
    void ensureRightPaneCreated();
    void updateState();

public slots:
    void openNewTab();
    void openNewWindow();
    void toggleViewMode(bool);
    void toggleOrientation(bool);
    void toggleSortColumn(bool);
    void toggleSortOrder(bool descending);
    void onSelectionChanged();

public:
    DualPaneWidget::Pane activePane;
    bool dualPaneModeEnabled;
    FileManagerWidget *panes[2];
    QVBoxLayout *layout;
    QSplitter *splitter;
    QAction *actions[DualPaneWidget::ActionCount];

    QActionGroup *viewModeGroup;
    QActionGroup *sortByGroup;

private:
    DualPaneWidget *q_ptr;
};

} // namespace FileManager

#endif // DUALPANEWIDGET_P_H
