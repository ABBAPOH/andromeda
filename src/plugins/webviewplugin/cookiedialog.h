#ifndef COOKIEDIALOG_H
#define COOKIEDIALOG_H

#include <QtGui/QDialog>

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
