#include "qimageviewsettings.h"
#include "qimageviewsettings_p.h"

#include <QApplication>

#include "qimageview.h"
#include "qimageview_p.h"

void QImageViewSettingsPrivate::addView(QImageView *view)
{
    Q_ASSERT(!views.contains(view));
    views.append(view);
}

void QImageViewSettingsPrivate::removeView(QImageView *view)
{
    Q_ASSERT(views.contains(view));
    views.removeOne(view);
}

void QImageViewSettingsPrivate::updateViews()
{
    foreach (QImageView *view, views) {
        view->viewport()->update();
    }
}

QImageViewSettings::QImageViewSettings() :
    d_ptr(new QImageViewSettingsPrivate)
{
    Q_D(QImageViewSettings);

    d->imageBackgroundType = Chess;
    d->imageBackgroundColor = qApp->palette().color(QPalette::Base);
    d->backgroundColor = qApp->palette().color(QPalette::Window).darker(150);
}

QImageViewSettings::~QImageViewSettings()
{
    delete d_ptr;
}

Q_GLOBAL_STATIC(QImageViewSettings, static_instance)

QImageViewSettings * QImageViewSettings::globalSettings()
{
    return static_instance();
}

QColor QImageViewSettings::imageBackgroundColor() const
{
    Q_D(const QImageViewSettings);

    return d->imageBackgroundColor;
}

void QImageViewSettings::setImageBackgroundColor(const QColor &color)
{
    Q_D(QImageViewSettings);

    if (d->imageBackgroundColor != color) {
        d->imageBackgroundColor = color;
        d->updateViews();
    }
}

QImageViewSettings::ImageBackgroundType QImageViewSettings::imageBackgroundType() const
{
    Q_D(const QImageViewSettings);

    return d->imageBackgroundType;
}

void QImageViewSettings::setiImageBackgroundType(QImageViewSettings::ImageBackgroundType type)
{
    Q_D(QImageViewSettings);

    if (d->imageBackgroundType != type) {
        d->imageBackgroundType = type;
        d->updateViews();
    }
}

QColor QImageViewSettings::backgroundColor() const
{
    Q_D(const QImageViewSettings);

    return d->backgroundColor;
}

void QImageViewSettings::setBackgroundColor(const QColor &color)
{
    Q_D(QImageViewSettings);

    if (d->backgroundColor != color) {
        d->backgroundColor = color;
        d->updateViews();
    }
}

bool QImageViewSettings::useOpenGL() const
{
    return d_func()->useOpenGL;
}

void QImageViewSettings::setUseOpenGL(bool yes)
{
    Q_D(QImageViewSettings);

    if (d->useOpenGL != yes) {
        d->useOpenGL = yes;

        foreach (QImageView *view, d->views) {
            view->d_func()->recreateViewport(d->useOpenGL);
        }

    }
}
