#include "cookieexceptionsdialog.h"
#include "ui_cookieexceptionsdialog.h"

#include "cookiemodel.h"
#include "cookieexceptionsmodel.h"

#if QT_VERSION >= 0x050000
#include <QtCore/QSortFilterProxyModel>
#include <QtWidgets/QCompleter>
#else
#include <QtGui/QSortFilterProxyModel>
#include <QtGui/QCompleter>
#endif

CookieExceptionsDialog::CookieExceptionsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CookieExceptionsDialog),
    m_cookieJar(0)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Sheet);
    connect(ui->removeButton, SIGNAL(clicked()), this, SLOT(remove()));
    connect(ui->removeAllButton, SIGNAL(clicked()), this, SLOT(removeAll()));
    m_exceptionsModel = new CookieExceptionsModel(this);
    m_proxyModel = new QSortFilterProxyModel(this);
    m_proxyModel->setSourceModel(m_exceptionsModel);
    connect(ui->search, SIGNAL(textChanged(QString)),
            m_proxyModel, SLOT(setFilterFixedString(QString)));
    ui->exceptionTable->setModel(m_proxyModel);

    CookieModel *cookieModel = new CookieModel(this);
    ui->domainLineEdit->setCompleter(new QCompleter(cookieModel, ui->domainLineEdit));

    connect(ui->domainLineEdit, SIGNAL(textChanged(QString)),
            this, SLOT(textChanged(QString)));
    connect(ui->blockButton, SIGNAL(clicked()), this, SLOT(block()));
    connect(ui->allowButton, SIGNAL(clicked()), this, SLOT(allow()));
    connect(ui->allowForSessionButton, SIGNAL(clicked()), this, SLOT(allowForSession()));

    adjustColumns();
}

CookieExceptionsDialog::~CookieExceptionsDialog()
{
    delete ui;
}

CookieJar * CookieExceptionsDialog::cookieJar() const
{
    return m_cookieJar;
}

void CookieExceptionsDialog::setCookieJar(CookieJar *cookieJar)
{
    if (!cookieJar)
        return;

    if (m_cookieJar == cookieJar)
        return;

    m_cookieJar = cookieJar;
    m_exceptionsModel->setCookeJar(m_cookieJar);
}

void CookieExceptionsDialog::textChanged(const QString &text)
{
    bool enabled = !text.isEmpty();
    ui->blockButton->setEnabled(enabled);
    ui->allowButton->setEnabled(enabled);
    ui->allowForSessionButton->setEnabled(enabled);
}

void CookieExceptionsDialog::remove()
{
}

void CookieExceptionsDialog::removeAll()
{
}

void CookieExceptionsDialog::block()
{
    QString text = ui->domainLineEdit->text();
    if (text.isEmpty())
        return;

    m_exceptionsModel->beginResetModel();
    m_exceptionsModel->m_blockedCookies.append(text);
    m_cookieJar->setBlockedCookies(m_exceptionsModel->m_blockedCookies);
    m_exceptionsModel->endResetModel();
}

void CookieExceptionsDialog::allow()
{
    QString text = ui->domainLineEdit->text();
    if (text.isEmpty())
        return;

    m_exceptionsModel->beginResetModel();
    m_exceptionsModel->m_allowedCookies.append(text);
    m_cookieJar->setAllowedCookies(m_exceptionsModel->m_allowedCookies);
    m_exceptionsModel->endResetModel();
}

void CookieExceptionsDialog::allowForSession()
{
    QString text = ui->domainLineEdit->text();
    if (text.isEmpty())
        return;

    m_exceptionsModel->beginResetModel();
    m_exceptionsModel->m_sessionCookies.append(text);
    m_cookieJar->setAllowForSessionCookies(m_exceptionsModel->m_sessionCookies);
    m_exceptionsModel->endResetModel();
}

void CookieExceptionsDialog::adjustColumns()
{
    QFont f = font();
    f.setPointSize(10);
    QFontMetrics fm(f);
    int height = fm.height() + fm.height()/3;
    ui->exceptionTable->verticalHeader()->setDefaultSectionSize(height);
    ui->exceptionTable->verticalHeader()->setMinimumSectionSize(-1);
    for (int i = 0; i < m_exceptionsModel->columnCount(); ++i){
        int header = ui->exceptionTable->horizontalHeader()->sectionSizeHint(i);
        switch (i) {
        case 0:
            header = fm.width(QLatin1String("averagebiglonghost.domain.com"));
            break;
        case 1:
            header = fm.width(QLatin1String("Allow For Session"));
            break;
        }
        int buffer = fm.width(QLatin1String("xx"));
        header += buffer;
        ui->exceptionTable->horizontalHeader()->resizeSection(i, header);
    }
}
