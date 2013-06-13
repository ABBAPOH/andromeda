#ifndef COOKIEDIALOG_H
#define COOKIEDIALOG_H

#include <QtCore/qglobal.h>

#if QT_VERSION >= 0x050000
#include <QtWidgets/QDialog>
#else
#include <QtGui/QDialog>
#endif

class CookieJar;
class CookieModel;
class QSortFilterProxyModel;

namespace Ui {
class CookieDialog;
} // namespace Ui

class CookieDialog : public QDialog
{
    Q_OBJECT
    Q_DISABLE_COPY(CookieDialog)

public:
    explicit CookieDialog(QWidget *parent = 0);
    ~CookieDialog();

    CookieJar *cookieJar() const;
    void setCookieJar(CookieJar *cookieJar);

public slots:
    void remove();
    void removeAll();

private:
    void adjustColumns();

private:
    Ui::CookieDialog *ui;
    CookieJar *m_cookieJar;
    CookieModel *m_model;
    QSortFilterProxyModel *m_proxy;
};

#endif // COOKIEDIALOG_H
