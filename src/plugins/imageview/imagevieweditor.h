#ifndef IMAGEVIEWEDITOR_H
#define IMAGEVIEWEDITOR_H

#include "imageview_global.h"

#include <guisystem/abstracteditor.h>
#include <guisystem/abstracteditorfactory.h>

class QImageView;

namespace ImageView {

class IMAGEVIEW_EXPORT ImageViewEditor : public GuiSystem::AbstractEditor
{
    Q_OBJECT
    Q_DISABLE_COPY(ImageViewEditor)

public:
    explicit ImageViewEditor(QWidget *parent = 0);

    void open(const QUrl &url);
    QUrl url() const;

    QIcon icon() const;
    QString title() const;
    QString windowTitle() const;

protected:
    void resizeEvent(QResizeEvent *);

private:
    QImageView *m_view;
    QUrl m_url;
};

class ImageViewEditorFactory : public GuiSystem::AbstractEditorFactory
{
public:
    explicit ImageViewEditorFactory(QObject *parent = 0);

    QByteArray id() const;
    QStringList mimeTypes() const;

protected:
    GuiSystem::AbstractEditor *createEditor(QWidget *parent);
};

} // namespace HelloWorld

#endif // IMAGEVIEWEDITOR_H
