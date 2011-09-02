#include "filecopyreplacedialog.h"
#include "ui_filecopyreplacedialog.h"

using namespace FileManagerPlugin;

FileCopyReplaceDialog::FileCopyReplaceDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FileCopyReplaceDialog)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::WindowTitleHint/*| Qt::WindowSystemMenuHint*/ | Qt::WindowMinMaxButtonsHint);
    connect(ui->skipButton, SIGNAL(clicked()), SLOT(onButtonClick()));
    connect(ui->cancelButton, SIGNAL(clicked()), SLOT(onButtonClick()));
    connect(ui->overwriteButton, SIGNAL(clicked()), SLOT(onButtonClick()));
}

FileCopyReplaceDialog::~FileCopyReplaceDialog()
{
    delete ui;
}

void FileCopyReplaceDialog::setIcon(const QIcon &icon)
{
    ui->iconLabel->setPixmap(icon.pixmap(32));
}

void FileCopyReplaceDialog::setMessage(const QString &message)
{
    ui->messageLabel->setText(message);
}

void FileCopyReplaceDialog::onButtonClick()
{
    QPushButton *button = qobject_cast<QPushButton *>(sender());
    if (button == ui->skipButton) {
        if (ui->checkBoxApplyToAll->isChecked())
            emit skipAll();
        else
            emit skip();
    } else if (button == ui->cancelButton) {
        emit cancelAll();
    } else if (button == ui->overwriteButton) {
        if (ui->checkBoxApplyToAll->isChecked())
            emit overwriteAll();
        else
            emit overwrite();
    }
    close();
}
