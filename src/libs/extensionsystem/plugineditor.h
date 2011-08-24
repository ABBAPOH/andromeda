#ifndef PLUGINEDITOR_H
#define PLUGINEDITOR_H

#include <QtGui/QWidget>

class QStringListModel;

namespace Ui {
    class PluginEditor;
}

namespace ExtensionSystem
{

class MutablePluginSpec;
class PluginSpec;

class PluginEditor : public QWidget
{
    Q_OBJECT

public:
    explicit PluginEditor(QWidget *parent = 0);
    ~PluginEditor();

    PluginSpec *pluginSpec();
    void setPluginSpec(MutablePluginSpec *spec);

public slots:
    void apply();
    void reset();

private:
    Ui::PluginEditor *ui;
    MutablePluginSpec *m_pluginSpec;
    QStringListModel *dependenciesModel;
};

} // namespace ExtensionSystem

#endif // PLUGINEDITOR_H
