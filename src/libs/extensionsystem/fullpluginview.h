#ifndef FULLPLUGINVIEW_H
#define FULLPLUGINVIEW_H

#include "extensionsystem_global.h"

#include <QtGui/QDialog>

namespace Ui {
    class FullPluginView;
}

class QDataWidgetMapper;
class QModelIndex;

namespace ExtensionSystem {

class PluginViewModel;
class EXTENSIONSYSTEM_EXPORT FullPluginView : public QDialog
{
    Q_OBJECT

public:
    explicit FullPluginView(QWidget *parent = 0);
    ~FullPluginView();
    void setModel(PluginViewModel * model);

public slots:
    void setIndex(const QModelIndex &index);

private:
    Ui::FullPluginView *ui;
    QDataWidgetMapper * mapper;
    PluginViewModel * model;
};

} // namespace ExtensionSystem

#endif // FULLPLUGINVIEW_H
