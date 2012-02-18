#include "filecopyerrordialog.h"
#include "ui_filecopyerrordialog.h"

FileCopyErrorDialog::FileCopyErrorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FileCopyErrorDialog)
{
    ui->setupUi(this);

    connect(ui->buttonBox, SIGNAL(clicked(QAbstractButton*)), SLOT(onButtonClick(QAbstractButton*)));
}

FileCopyErrorDialog::~FileCopyErrorDialog()
{
    delete ui;
}

void FileCopyErrorDialog::setIcon(const QIcon &icon)
{
    ui->iconLabel->setPixmap(icon.pixmap(32));
}

void FileCopyErrorDialog::setMessage(const QString &message)
{
    ui->messageLabel->setText(message);
}

void FileCopyErrorDialog::onButtonClick(QAbstractButton* btn)
{
    QDialogButtonBox::StandardButtons button = ui->buttonBox->standardButton(btn);
    if (button & QDialogButtonBox::Abort)
        emit abort();
    else if (button & QDialogButtonBox::Ignore)
        emit ignore();
    else if (button & QDialogButtonBox::Retry)
        emit retry();
}
