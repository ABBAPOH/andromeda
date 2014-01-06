#include "generalsettingswidget.h"
#include "ui_generalsettingswidget.h"

#include "application.h"

using namespace Andromeda;

/*!
    \class Andromeda::GeneralSettingsWidget
    Widget that displays main Andromeda settings.
*/

/*!
    Creates GeneralSettingsWidget with the given \a parent.
*/
GeneralSettingsWidget::GeneralSettingsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GeneralSettingsWidget)
{
    ui->setupUi(this);

    ui->showTrayIcon->setChecked(qApp->isTrayIconVisible());

    connect(ui->showTrayIcon, SIGNAL(toggled(bool)), SLOT(onShowTrayIconChecked(bool)));
}

/*!
    Destroys GeneralSettingsWidget.
*/
GeneralSettingsWidget::~GeneralSettingsWidget()
{
    delete ui;
}

/*!
    \internal
*/
void GeneralSettingsWidget::onShowTrayIconChecked(bool checked)
{
    qApp->setTrayIconVisible(checked);
}
