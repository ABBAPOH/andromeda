#ifndef QIMAGEVIEWRESIZEDIALOG_H
#define QIMAGEVIEWRESIZEDIALOG_H

#include "qimageview_global.h"

#include <QtGui/QDialog>

namespace Ui {
class QImageViewResizeDialog;
}

class QIMAGEVIEW_EXPORT QImageViewResizeDialog : public QDialog
{
    Q_OBJECT
    Q_PROPERTY(QSize imageSize READ imageSize WRITE setImageSize NOTIFY imageSizeChanged)

public:
    explicit QImageViewResizeDialog(QWidget *parent = 0);
    ~QImageViewResizeDialog();

    QSize imageSize() const;
    void setImageSize(const QSize &size);

signals:
    void imageSizeChanged(const QSize &);

private slots:
    void fitIn(int);
    void widthChanged(int);
    void heightChanged(int);
    void saveProportionsClicked(bool);

private:
    qreal factor() const;

private:
    Ui::QImageViewResizeDialog *ui;

    QSize m_size;
    QSize m_currentSize;
    bool m_ignoreSignals;
};

#endif // QIMAGEVIEWRESIZEDIALOG_H
