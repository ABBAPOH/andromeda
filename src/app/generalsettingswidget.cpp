#include "generalsettingswidget.h"
#include "ui_generalsettingswidget.h"

#include "application.h"

using namespace Andromeda;

GeneralSettingsWidget::GeneralSettingsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GeneralSettingsWidget)
{
    ui->setupUi(this);

    ui->showTrayIcon->setChecked(qApp->isTrayIconVisible());

    connect(ui->showTrayIcon, SIGNAL(toggled(bool)), SLOT(onShowTrayIconChecked(bool)));
}

GeneralSettingsWidget::~GeneralSettingsWidget()
{
    delete ui;
}

void GeneralSettingsWidget::onShowTrayIconChecked(bool checked)
{
    qApp->setTrayIconVisible(checked);
}
