#include "fileinfodialog.h"
#include "ui_fileinfodialog.h"

#include <QtCore/QDateTime>

FileInfoDialog::FileInfoDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FileInfoDialog)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Window);
}

FileInfoDialog::~FileInfoDialog()
{
    delete ui;
}

QFileInfo FileInfoDialog::fileInfo() const
{
    return m_fileInfo;
}

void FileInfoDialog::setFileInfo(const QFileInfo &info)
{
    m_fileInfo = info;
    updateUi();
}

static QString sizeToString(qint64 size)
{
    if (size > (qint64)1024*1024*1024*1024)
        return QString::number(size/((qint64)1024*1024*1024*1024)) + "TB";
    if (size > 1024*1024*1024)
        return QString::number(size/(1024*1024*1024)) + "GB";
    if (size > 1024*1024)
        return QString::number(size/(1024*1024)) + "MB";
    if (size > 1024)
        return QString::number(size/1024) + "kB";
    return QString::number(size) + "b";
}

#include <QFileIconProvider>
void FileInfoDialog::updateUi()
{
    ui->label->setPixmap(QFileIconProvider().icon(m_fileInfo).pixmap(32));
    ui->nameLabel->setText(m_fileInfo.fileName());
    ui->mimeTypeLabel->setText(QString());
    ui->sizeLabel->setText(sizeToString(m_fileInfo.size()));
    ui->locationLabel->setText(m_fileInfo.path());
    ui->createdLabel->setText(m_fileInfo.created().toString(Qt::SystemLocaleShortDate));
    ui->modifiedLabel->setText(m_fileInfo.lastModified().toString(Qt::SystemLocaleShortDate));
    ui->acceccedLabel->setText(m_fileInfo.lastRead().toString(Qt::SystemLocaleShortDate));
}
