#include "cookiedialog.h"
#include "ui_cookiedialog.h"

#include <QtCore/QDateTime>
#include <QtGui/QSortFilterProxyModel>

#include "cookiemodel.h"

CookieDialog::CookieDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CookieDialog),
    m_cookieJar(0)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Sheet);

    m_model = new CookieModel(this);
    m_proxy = new QSortFilterProxyModel(this);
    m_proxy->setSourceModel(m_model);
    ui->cookiesTable->setModel(m_proxy);

    connect(ui->search, SIGNAL(textChanged(QString)),
            m_proxy, SLOT(setFilterFixedString(QString)));
    connect(ui->removeButton, SIGNAL(clicked()), this, SLOT(remove()));
    connect(ui->removeAllButton, SIGNAL(clicked()), this, SLOT(removeAll()));

    adjustColumns();
}

CookieDialog::~CookieDialog()
{
    delete ui;
}

CookieJar * CookieDialog::cookieJar() const
{
    return m_cookieJar;
}

void CookieDialog::setCookieJar(CookieJar *cookieJar)
{
    if (!cookieJar)
        return;

    if (m_cookieJar == cookieJar)
        return;

    m_cookieJar = cookieJar;
    m_model->setCookeJar(m_cookieJar);
}

void CookieDialog::remove()
{
    QModelIndex currentIndex = ui->cookiesTable->currentIndex();
    QModelIndex rootIndex = ui->cookiesTable->rootIndex();

    int row = currentIndex.row();
    m_proxy->removeRow(row, rootIndex);

    QModelIndex idx = m_proxy->index(row, 0, rootIndex);
    if (!idx.isValid())
        idx = m_proxy->index(row - 1, 0, rootIndex);

    ui->cookiesTable->selectionModel()->select(idx, QItemSelectionModel::SelectCurrent | QItemSelectionModel::Rows);
}

void CookieDialog::removeAll()
{
    if (m_proxy) {
        QModelIndex rootIndex = ui->cookiesTable->rootIndex();
        m_proxy->removeRows(0, m_proxy->rowCount(rootIndex), rootIndex);
    }
}

void CookieDialog::adjustColumns()
{
    QFont f = font();
    f.setPointSize(10);
    QFontMetrics fm(f);
    int height = fm.height() + fm.height() / 3;
    ui->cookiesTable->verticalHeader()->setDefaultSectionSize(height);
    ui->cookiesTable->verticalHeader()->setMinimumSectionSize(-1);

    for (int i = 0; i < m_model->columnCount(); ++i) {
        int header = ui->cookiesTable->horizontalHeader()->sectionSizeHint(i);
        switch (i) {
        case 0:
            header = fm.width(QLatin1String("averagehost.domain.com"));
            break;
        case 1:
            header = fm.width(QLatin1String("_session_id"));
            break;
        case 4:
            header = fm.width(QDateTime::currentDateTime().toString(Qt::LocalDate));
            break;
        }
        int buffer = fm.width(QLatin1String("xx"));
        header += buffer;
        ui->cookiesTable->horizontalHeader()->resizeSection(i, header);
    }
}
