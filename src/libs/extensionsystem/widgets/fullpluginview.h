#ifndef FULLPLUGINVIEW_H
#define FULLPLUGINVIEW_H

#include "../extensionsystem_global.h"

#include <QtGui/QDialog>

class QDataWidgetMapper;
class QModelIndex;

namespace Ui {
    class FullPluginView;
}

namespace ExtensionSystem {

class PluginViewModel;
class EXTENSIONSYSTEM_EXPORT FullPluginView : public QDialog
{
    Q_OBJECT
    Q_DISABLE_COPY(FullPluginView)

public:
    explicit FullPluginView(QWidget *parent = 0);
    ~FullPluginView();

    PluginViewModel *model() const;
    void setModel(PluginViewModel *m_model);

public slots:
    void setIndex(const QModelIndex &index);

private:
    Ui::FullPluginView *ui;
    QDataWidgetMapper *m_mapper;
    PluginViewModel *m_model;
};

} // namespace ExtensionSystem

#endif // FULLPLUGINVIEW_H
