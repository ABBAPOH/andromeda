#ifndef QIMAGEVIEWSETTINGS_H
#define QIMAGEVIEWSETTINGS_H

#include "qimageview_global.h"

#include <QColor>

class QImageViewSettingsPrivate;
class QIMAGEVIEW_EXPORT QImageViewSettings
{
    Q_DECLARE_PRIVATE(QImageViewSettings)

public:
    enum ImageBackgroundType {
        None,
        Chess,
        SolidColor
    };

    QImageViewSettings();
    ~QImageViewSettings();

    static QImageViewSettings *globalSettings();

    QColor imageBackgroundColor() const;
    void setImageBackgroundColor(const QColor &color);

    ImageBackgroundType imageBackgroundType() const;
    void setiImageBackgroundType(ImageBackgroundType type);

    QColor backgroundColor() const;
    void setBackgroundColor(const QColor &color);

    bool useOpenGL() const;
    void setUseOpenGL(bool yes);

private:
    QImageViewSettingsPrivate *d_ptr;

    friend class QImageView;
};

#endif // QIMAGEVIEWSETTINGS_H
