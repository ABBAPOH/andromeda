#include "filecopyreplacedialog.h"
#include "ui_filecopyreplacedialog.h"

FileCopyReplaceDialog::FileCopyReplaceDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FileCopyReplaceDialog)
{
    ui->setupUi(this);
}

FileCopyReplaceDialog::~FileCopyReplaceDialog()
{
    delete ui;
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
            emit overwrite();
        else
            emit overwriteAll();
    }
    close();
}
