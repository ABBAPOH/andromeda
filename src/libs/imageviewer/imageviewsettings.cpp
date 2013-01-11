#include "imageviewsettings.h"
#include "imageviewsettings_p.h"

#include <QApplication>

#include "imageview.h"
#include "imageview_p.h"

void ImageViewSettingsPrivate::addView(ImageView *view)
{
    Q_ASSERT(!views.contains(view));
    views.append(view);
}

void ImageViewSettingsPrivate::removeView(ImageView *view)
{
    Q_ASSERT(views.contains(view));
    views.removeOne(view);
}

void ImageViewSettingsPrivate::updateViews()
{
    foreach (ImageView *view, views) {
        view->viewport()->update();
    }
}

ImageViewSettings::ImageViewSettings() :
    d_ptr(new ImageViewSettingsPrivate)
{
    Q_D(ImageViewSettings);

    d->imageBackgroundType = Chess;
    d->imageBackgroundColor = qApp->palette().color(QPalette::Base);
    d->backgroundColor = qApp->palette().color(QPalette::Window).darker(150);
}

ImageViewSettings::~ImageViewSettings()
{
    delete d_ptr;
}

Q_GLOBAL_STATIC(ImageViewSettings, static_instance)

ImageViewSettings * ImageViewSettings::globalSettings()
{
    return static_instance();
}

QColor ImageViewSettings::imageBackgroundColor() const
{
    Q_D(const ImageViewSettings);

    return d->imageBackgroundColor;
}

void ImageViewSettings::setImageBackgroundColor(const QColor &color)
{
    Q_D(ImageViewSettings);

    if (d->imageBackgroundColor != color) {
        d->imageBackgroundColor = color;
        d->updateViews();
    }
}

ImageViewSettings::ImageBackgroundType ImageViewSettings::imageBackgroundType() const
{
    Q_D(const ImageViewSettings);

    return d->imageBackgroundType;
}

void ImageViewSettings::setiImageBackgroundType(ImageViewSettings::ImageBackgroundType type)
{
    Q_D(ImageViewSettings);

    if (d->imageBackgroundType != type) {
        d->imageBackgroundType = type;
        d->updateViews();
    }
}

QColor ImageViewSettings::backgroundColor() const
{
    Q_D(const ImageViewSettings);

    return d->backgroundColor;
}

void ImageViewSettings::setBackgroundColor(const QColor &color)
{
    Q_D(ImageViewSettings);

    if (d->backgroundColor != color) {
        d->backgroundColor = color;
        d->updateViews();
    }
}

bool ImageViewSettings::useOpenGL() const
{
    return d_func()->useOpenGL;
}

void ImageViewSettings::setUseOpenGL(bool yes)
{
    Q_D(ImageViewSettings);

    if (d->useOpenGL != yes) {
        d->useOpenGL = yes;

        foreach (ImageView *view, d->views) {
            view->d_func()->recreateViewport(d->useOpenGL);
        }

    }
}
