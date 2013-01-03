#include "permissionswidget.h"
#include "ui_permissionswidget.h"

#include <QtGui/QRegExpValidator>

using namespace FileManager;

/*!
    \class FileManager::PermissionsWidget

    \brief PermissionsWidget is a simple widget for editing file permissions.

    \image html permissionswidget.png
*/

/*!
    Creates PermissionsWidget with the given \a parent.
*/
PermissionsWidget::PermissionsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PermissionsWidget)
{
    ui->setupUi(this);

    QRegExpValidator *validator = new QRegExpValidator(ui->numericPermissions);
    validator->setRegExp(QRegExp("[0-7][0-7][0-7]"));
    ui->numericPermissions->setValidator(validator);

    connect(ui->readOwner, SIGNAL(toggled(bool)), SLOT(buttonPressed()));
    connect(ui->readGroup, SIGNAL(toggled(bool)), SLOT(buttonPressed()));
    connect(ui->readOther, SIGNAL(toggled(bool)), SLOT(buttonPressed()));

    connect(ui->writeOwner, SIGNAL(toggled(bool)), SLOT(buttonPressed()));
    connect(ui->writeGroup, SIGNAL(toggled(bool)), SLOT(buttonPressed()));
    connect(ui->writeOther, SIGNAL(toggled(bool)), SLOT(buttonPressed()));

    connect(ui->exeOwner, SIGNAL(toggled(bool)), SLOT(buttonPressed()));
    connect(ui->exeGroup, SIGNAL(toggled(bool)), SLOT(buttonPressed()));
    connect(ui->exeOther, SIGNAL(toggled(bool)), SLOT(buttonPressed()));

    connect(ui->numericPermissions, SIGNAL(returnPressed()), SLOT(numericChanged()));
}

/*!
    Destroys PermissionsWidget.
*/
PermissionsWidget::~PermissionsWidget()
{
    delete ui;
}

/*!
    \property PermissionsWidget::fileInfo
    This property holds current file or folder displayed in the PermissionsWidget.
*/
QFileInfo PermissionsWidget::fileInfo() const
{
    return m_fileInfo;
}

void PermissionsWidget::setFileInfo(const QFileInfo &info)
{
    if (m_fileInfo == info)
        return;

    m_fileInfo = info;

    updateWidget();
    emit fileInfoChanged(info);
}

static uint unixPermissions(QFile::Permissions permissions)
{
    uint result = 0;
    result += (permissions & 0xF);
    result += (permissions & 0xF0) >> 1;
    result += (permissions & 0xF000) >> 6;

    return result;
}

static QFile::Permissions permissions(uint unixPermissions)
{
    uint result = 0;
    result += unixPermissions & 0x7;
    result += (unixPermissions & 070) << 1;
    result += (unixPermissions & 0700) << 2;
    result += (unixPermissions & 0700) << 6;

    return QFile::Permissions(result);
}

static QString textPermissionsHelper(uint permissions)
{
    QChar r = (permissions & 4) ? 'r' : '-';
    QChar w = (permissions & 2) ? 'w' : '-';
    QChar x = (permissions & 1) ? 'x' : '-';

    return QString("%1%2%3").arg(r).arg(w).arg(x);
}

static QString textPermissions(uint unixPermissions)
{
    return QString("%1%2%3").arg(textPermissionsHelper((unixPermissions >> 6) & 0x7)).
            arg(textPermissionsHelper((unixPermissions >> 3) & 0x7)).
            arg(textPermissionsHelper((unixPermissions) & 0x7));
}

void PermissionsWidget::updateWidget()
{
    uint ownerId = m_fileInfo.ownerId();
    uint groupId = m_fileInfo.groupId();

    if (ownerId != (uint)-2)
        ui->user->setText(tr("%1 (%2)").arg(m_fileInfo.owner()).arg(ownerId));
    else
        ui->user->setText(tr("%1").arg(m_fileInfo.owner()));

    if (groupId != (uint)-2)
        ui->group->setText(tr("%1 (%2)").arg(m_fileInfo.group()).arg(groupId));
    else
        ui->group->setText(tr("%1").arg(m_fileInfo.group()));

    QFile::Permissions permissions = m_fileInfo.permissions();

    ui->readOwner->setChecked(permissions & QFile::ReadOwner);
    ui->readGroup->setChecked(permissions & QFile::ReadGroup);
    ui->readOther->setChecked(permissions & QFile::ReadOther);

    ui->writeOwner->setChecked(permissions & QFile::WriteOwner);
    ui->writeGroup->setChecked(permissions & QFile::WriteGroup);
    ui->writeOther->setChecked(permissions & QFile::WriteOther);

    ui->exeOwner->setChecked(permissions & QFile::ExeOwner);
    ui->exeGroup->setChecked(permissions & QFile::ExeGroup);
    ui->exeOther->setChecked(permissions & QFile::ExeOther);

    uint unixPermissions = ::unixPermissions(permissions);
    ui->textPermissions->setText(textPermissions(unixPermissions));
    ui->numericPermissions->setText(QString("%1").arg(unixPermissions, 3, 8, QChar('0')));
}

void PermissionsWidget::buttonPressed()
{
    QFile::Permissions permissions = 0;

    permissions |= ui->readOwner->isChecked() ? QFile::ReadOwner : (QFile::Permission)0;
    permissions |= ui->readGroup->isChecked() ? QFile::ReadGroup : (QFile::Permission)0;
    permissions |= ui->readOther->isChecked() ? QFile::ReadOther : (QFile::Permission)0;

    permissions |= ui->writeOwner->isChecked() ? QFile::WriteOwner : (QFile::Permission)0;
    permissions |= ui->writeGroup->isChecked() ? QFile::WriteGroup : (QFile::Permission)0;
    permissions |= ui->writeOther->isChecked() ? QFile::WriteOther : (QFile::Permission)0;

    permissions |= ui->exeOwner->isChecked() ? QFile::ExeOwner : (QFile::Permission)0;
    permissions |= ui->exeGroup->isChecked() ? QFile::ExeGroup : (QFile::Permission)0;
    permissions |= ui->exeOther->isChecked() ? QFile::ExeOther : (QFile::Permission)0;

    QFile::setPermissions(m_fileInfo.absoluteFilePath(), permissions);
    m_fileInfo.refresh();

    updateWidget();
}

void PermissionsWidget::numericChanged()
{
    uint unixPermissions = ui->numericPermissions->text().toUInt(0, 8);

    QFile::Permissions permissions = ::permissions(unixPermissions);

    QFile::setPermissions(m_fileInfo.absoluteFilePath(), permissions);
    m_fileInfo.refresh();

    updateWidget();
}
