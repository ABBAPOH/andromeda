#include "fileinfodialog.h"
#include "ui_fileinfodialog.h"

#include "directorydetails.h"

#include <QtCore/QDateTime>
#include <QtGui/QFileIconProvider>

FileInfoDialog::FileInfoDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FileInfoDialog),
    m_directoryDetails(0)
{
    ui->setupUi(this);

    setAttribute(Qt::WA_DeleteOnClose, true);
    setWindowFlags(Qt::Window);

    connect(ui->userPermissionsComboBox, SIGNAL(activated(int)), SLOT(onActivatedUser(int)));
    connect(ui->groupPermissionsComboBox, SIGNAL(activated(int)), SLOT(onActivatedGroup(int)));
    connect(ui->otherPermissionsComboBox, SIGNAL(activated(int)), SLOT(onActivatedOther(int)));
}

FileInfoDialog::~FileInfoDialog()
{
    if (m_directoryDetails) {
        if (m_directoryDetails->isRunning())
            m_directoryDetails->stopProcessing();

        m_directoryDetails->wait();
    }

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
    if (m_directoryDetails)
        delete m_directoryDetails;
    m_directoryDetails = new DirectoryDetails(info.absoluteFilePath(), this);
    connect(m_directoryDetails, SIGNAL(finished()), this, SLOT(updateSize()));
    m_directoryDetails->start();
    updateUi();
}

static QString sizeToString(qint64 size)
{
    if (size > (qint64)1024*1024*1024*1024)
        return QObject::tr("%1 TB").arg(size/((qint64)1024*1024*1024*1024));
    if (size > 1024*1024*1024)
        return QObject::tr("%1 GB").arg(size/(1024*1024*1024));
    if (size > 1024*1024)
        return QObject::tr("%1 MB").arg(size/(1024*1024));
    if (size > 1024)
        return QObject::tr("%1 kB").arg(size/1024);
    return QObject::tr("%1 b").arg(size);
}

void FileInfoDialog::updateUi()
{
    ui->iconLabel->setPixmap(QFileIconProvider().icon(m_fileInfo).pixmap(32));
    ui->nameLabel->setText(m_fileInfo.fileName());
    ui->mimeTypeLabel->setText(QString());
    m_fileInfo.isDir() ? ui->sizeLabel->setText(tr("Calculating...")) : 
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

void FileInfoDialog::updateSize()
{
    int objCount = m_directoryDetails->totalFiles() + m_directoryDetails->totalFolders();
    qint64 totalSize = m_directoryDetails->totalSize();
    QString sizeLabel = tr("%1 bytes (%2) for %3 objects").arg(totalSize)
                                            .arg(sizeToString(totalSize))
                                            .arg(objCount);
    ui->sizeLabel->setText(sizeLabel);
}
