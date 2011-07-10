#ifndef PLUGINVIEW_H
#define PLUGINVIEW_H

#include "extensionsystem_global.h"

#include <QtCore/QModelIndex>
#include <QtGui/QDialog>

namespace Ui {
    class PluginView;
}

namespace ExtensionSystem {

class FullPluginView;

class EXTENSIONSYSTEM_EXPORT PluginView : public QDialog
{
    Q_OBJECT

public:
    explicit PluginView(QWidget *parent = 0);
    ~PluginView();

private:
    Ui::PluginView *ui;
    FullPluginView *m_fullPluginView;

private slots:
    void showFullInfo(const QModelIndex &index = QModelIndex());
};

} // namespace ExtensionSystem

#endif // PLUGINVIEW_H
