#ifndef IMAGEVIEWSETTINGS_H
#define IMAGEVIEWSETTINGS_H

#include "imageviewer_global.h"

#include <QColor>

class ImageViewSettingsPrivate;
class IMAGEVIEWER_EXPORT ImageViewSettings
{
    Q_DECLARE_PRIVATE(ImageViewSettings)

public:
    enum ImageBackgroundType {
        None,
        Chess,
        SolidColor
    };

    ImageViewSettings();
    ~ImageViewSettings();

    static ImageViewSettings *globalSettings();

    QColor imageBackgroundColor() const;
    void setImageBackgroundColor(const QColor &color);

    ImageBackgroundType imageBackgroundType() const;
    void setiImageBackgroundType(ImageBackgroundType type);

    QColor backgroundColor() const;
    void setBackgroundColor(const QColor &color);

    bool useOpenGL() const;
    void setUseOpenGL(bool yes);

private:
    ImageViewSettingsPrivate *d_ptr;

    friend class ImageView;
};

#endif // IMAGEVIEWSETTINGS_H
