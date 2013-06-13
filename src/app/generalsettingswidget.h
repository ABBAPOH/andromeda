#ifndef GENERALSETTINGSWIDGET_H
#define GENERALSETTINGSWIDGET_H

#include <QWidget>

namespace Ui {
class GeneralSettingsWidget;
}

namespace Andromeda {

class GeneralSettingsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GeneralSettingsWidget(QWidget *parent = 0);
    ~GeneralSettingsWidget();

private slots:
    void onShowTrayIconChecked(bool checked);

private:
    Ui::GeneralSettingsWidget *ui;
};

} // namespace Andromeda

#endif // GENERALSETTINGSWIDGET_H
