#ifndef PLUGINVIEW_H
#define PLUGINVIEW_H

#include "extensionsystem_global.h"

#include <QtGui/QDialog>
#include <QtCore/QModelIndex>

namespace Ui {
    class PluginView;
}

namespace ExtensionSystem {

class PluginManager;
class FullPluginView;
class EXTENSIONSYSTEM_EXPORT PluginView : public QDialog
{
    Q_OBJECT

public:
    explicit PluginView(QWidget *parent = 0);
    ~PluginView();

private:
    Ui::PluginView *ui;
    PluginManager *manager;
    FullPluginView * fullPluginView;

private slots:
    void showFullInfo(const QModelIndex & index = QModelIndex());
};

} // namespace ExtensionSystem

#endif // PLUGINVIEW_H
