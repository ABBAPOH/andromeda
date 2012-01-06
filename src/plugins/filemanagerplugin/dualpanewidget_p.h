#ifndef DUALPANEWIDGET_P_H
#define DUALPANEWIDGET_P_H

#include "dualpanewidget.h"

class QHBoxLayout;
class QActionGroup;

namespace FileManagerPlugin {

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
    void updateViewModeActions();
    void updateSortActions();
    void updateState();

public slots:
    void openNewTab();
    void openNewWindow();
    void toggleViewMode(bool);
    void toggleSortColumn(bool);
    void toggleSortOrder(bool descending);
    void onSelectionChanged();

public:
    DualPaneWidget::Pane activePane;
    bool dualPaneModeEnabled;
    FileManagerWidget *panes[2];
    QHBoxLayout *layout;
    FileManagerWidget::ViewMode viewMode;
    QAction *actions[DualPaneWidget::ActionCount];

    QActionGroup *viewModeGroup;
    QActionGroup *sortByGroup;

private:
    DualPaneWidget *q_ptr;
};

} // namespace FileManagerPlugin

#endif // DUALPANEWIDGET_P_H
