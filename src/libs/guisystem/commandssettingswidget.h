#ifndef COMMANDSSETTINGS_H
#define COMMANDSSETTINGS_H

#include "guisystem_global.h"

#include <QtGui/QWidget>

namespace Ui {
class CommandsSettingsWidget;
}

class ShortcutLineEdit;
class QItemSelection;
class QSortFilterProxyModel;

namespace GuiSystem {

class CommandsModel;

class GUISYSTEM_EXPORT CommandsSettingsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CommandsSettingsWidget(QWidget *parent = 0);
    ~CommandsSettingsWidget();

private slots:
    void onTextChanged(const QString &text);
    void onSelectionChanged(const QItemSelection &selection);
    void reset();
    void resetAll();

private:
    Ui::CommandsSettingsWidget *ui;
    CommandsModel *m_model;
    QSortFilterProxyModel *m_proxy;
};

} // namespace GuiSystem

#endif // COMMANDSSETTINGS_H
