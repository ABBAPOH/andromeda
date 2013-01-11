#ifndef QIMAGEVIEWRESIZEDIALOG_H
#define QIMAGEVIEWRESIZEDIALOG_H

#include "imageviewer_global.h"

#include <QtGui/QDialog>

namespace Ui {
class ResizeDialog;
}

class IMAGEVIEWER_EXPORT ResizeDialog : public QDialog
{
    Q_OBJECT
    Q_PROPERTY(QSize imageSize READ imageSize WRITE setImageSize NOTIFY imageSizeChanged)

public:
    explicit ResizeDialog(QWidget *parent = 0);
    ~ResizeDialog();

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
    Ui::ResizeDialog *ui;

    QSize m_size;
    QSize m_currentSize;
    bool m_ignoreSignals;
};

#endif // QIMAGEVIEWRESIZEDIALOG_H
