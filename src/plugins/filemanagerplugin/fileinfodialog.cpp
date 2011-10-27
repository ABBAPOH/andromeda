#include "fileinfodialog.h"
#include "ui_fileinfodialog.h"

#include <QtCore/QDateTime>

FileInfoDialog::FileInfoDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FileInfoDialog)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Window);

    connect(ui->userPermissionsComboBox, SIGNAL(activated(int)), SLOT(onActivatedUser(int)));
    connect(ui->groupPermissionsComboBox, SIGNAL(activated(int)), SLOT(onActivatedGroup(int)));
    connect(ui->otherPermissionsComboBox, SIGNAL(activated(int)), SLOT(onActivatedOther(int)));
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
    m_driveInfo = QDriveInfo(m_fileInfo.absoluteFilePath());
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
    ui->iconLabel->setPixmap(QFileIconProvider().icon(m_fileInfo).pixmap(32));
    ui->nameLabel->setText(m_fileInfo.fileName());
    ui->mimeTypeLabel->setText(QString());
    ui->sizeLabel->setText(sizeToString(m_fileInfo.size()));
    ui->locationLabel->setText(m_fileInfo.path());
    ui->createdLabel->setText(m_fileInfo.created().toString(Qt::SystemLocaleShortDate));
    ui->modifiedLabel->setText(m_fileInfo.lastModified().toString(Qt::SystemLocaleShortDate));
    ui->acceccedLabel->setText(m_fileInfo.lastRead().toString(Qt::SystemLocaleShortDate));

    ui->driveLabel->setText(m_driveInfo.name());
    ui->mountPointLabel->setText(m_driveInfo.rootPath());
    ui->fileSystemLabel->setText(m_driveInfo.fileSystemName());
    ui->totalSizeLabel->setText(sizeToString(m_driveInfo.bytesTotal()));
    ui->availableSizeLabel->setText(sizeToString(m_driveInfo.bytesAvailable()));

    ui->userPermissionsComboBox->setCurrentIndex((m_fileInfo.permissions() & QFile::WriteOwner) ? 1 : 0);
    ui->groupPermissionsComboBox->setCurrentIndex((m_fileInfo.permissions() & QFile::WriteGroup) ? 1 : 0);
    ui->otherPermissionsComboBox->setCurrentIndex((m_fileInfo.permissions() & QFile::WriteOther) ? 1 : 0);
}

void FileInfoDialog::onActivatedUser(int i)
{
    m_fileInfo.refresh();
    QFile::Permissions flags = (i == 1) ? (QFile::WriteOwner | QFile::ReadOwner) : (QFile::ReadOwner);
    QFile::setPermissions(m_fileInfo.filePath(), (m_fileInfo.permissions() & 0x1FF) | flags );
}

void FileInfoDialog::onActivatedGroup(int i)
{
    m_fileInfo.refresh();
    QFile::Permissions flags = (i == 1) ? (QFile::WriteGroup | QFile::ReadGroup) : (QFile::ReadGroup);
    QFile::setPermissions(m_fileInfo.filePath(), (m_fileInfo.permissions() & 0xFC7) | flags );
}

void FileInfoDialog::onActivatedOther(int i)
{
    m_fileInfo.refresh();
    QFile::Permissions flags = (i == 1) ? (QFile::WriteOther | QFile::ReadOther) : (QFile::ReadOther);
    QFile::setPermissions(m_fileInfo.filePath(), (m_fileInfo.permissions() & 0xFF8) | flags );
}
