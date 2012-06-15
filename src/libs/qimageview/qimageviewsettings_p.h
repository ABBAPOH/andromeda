#ifndef QIMAGEVIEWSETTINGS_P_H
#define QIMAGEVIEWSETTINGS_P_H

#include "qimageviewsettings.h"

class QImageView;

class QImageViewSettingsPrivate
{
public:
    void addView(QImageView *view);
    void removeView(QImageView *view);

    void updateViews();

public:
    QImageViewSettings::ImageBackgroundType imageBackgroundType;
    QColor imageBackgroundColor;
    QColor backgroundColor;
    bool useOpenGL;

private:
    QList<QImageView *> views;

    friend class QImageViewSettings;
};

#endif // QIMAGEVIEWSETTINGS_P_H
