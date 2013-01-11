#ifndef PREFERENCESWIDGET_H
#define PREFERENCESWIDGET_H

#include "imageviewer_global.h"

#include <QWidget>

namespace Ui {
class PreferencesWidget;
}

class IMAGEVIEWER_EXPORT PreferencesWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PreferencesWidget(QWidget *parent = 0);
    ~PreferencesWidget();

private slots:
    void noBackgroundClicked(bool);
    void solidColorClicked(bool);
    void checkeredBackgroundClicked(bool);

    void backgroundColorChanged(const QColor &color);
    void imageColorChanged(const QColor &color);

    void useOpenGLClicked(bool);

private:
    Ui::PreferencesWidget *ui;
};

#endif // PREFERENCESWIDGET_H
