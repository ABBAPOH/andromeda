#ifndef QIMAGEVIEWSETTINGS_P_H
#define QIMAGEVIEWSETTINGS_P_H

#include "imageviewsettings.h"

namespace ImageViewer {

class ImageView;

class ImageViewSettingsPrivate
{
public:
    void addView(ImageView *view);
    void removeView(ImageView *view);

    void updateViews();

public:
    ImageViewSettings::ImageBackgroundType imageBackgroundType;
    QColor imageBackgroundColor;
    QColor backgroundColor;
    bool useOpenGL;

private:
    QList<ImageView *> views;

    friend class ImageViewSettings;
};

} // namespace ImageViewer

#endif // QIMAGEVIEWSETTINGS_P_H
