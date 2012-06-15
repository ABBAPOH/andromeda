#ifndef QIMAGEVIEWPREFERENCESWIDGET_H
#define QIMAGEVIEWPREFERENCESWIDGET_H

#include <QWidget>

namespace Ui {
class QImageViewPreferencesWidget;
}

class QImageViewPreferencesWidget : public QWidget
{
    Q_OBJECT

public:
    explicit QImageViewPreferencesWidget(QWidget *parent = 0);
    ~QImageViewPreferencesWidget();

private slots:
    void noBackgroundClicked(bool);
    void solidColorClicked(bool);
    void checkeredBackgroundClicked(bool);

    void backgroundColorChanged(const QColor &color);
    void imageColorChanged(const QColor &color);

    void useOpenGLClicked(bool);

private:
    Ui::QImageViewPreferencesWidget *ui;
};

#endif // QIMAGEVIEWPREFERENCESWIDGET_H
