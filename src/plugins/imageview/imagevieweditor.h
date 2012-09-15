#ifndef IMAGEVIEWEDITOR_H
#define IMAGEVIEWEDITOR_H

#include "imageview_global.h"

#include <guisystem/abstracteditor.h>
#include <guisystem/abstracteditorfactory.h>

class QImageView;
class QToolBar;
class QVBoxLayout;

namespace ImageView {

class ImageViewFile;

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

    GuiSystem::IFile *file() const;

protected:
    void setupUi();
    void registerActions();

private:
    QVBoxLayout *m_layout;
    QToolBar *m_toolBar;
    QImageView *m_view;
    QUrl m_url;
    ImageViewFile *m_file;

    friend class ImageViewFile;
};

class ImageViewEditorFactory : public GuiSystem::AbstractEditorFactory
{
public:
    explicit ImageViewEditorFactory(QObject *parent = 0);

    QByteArray id() const;
    QString name() const;
    QIcon icon() const;
    QStringList mimeTypes() const;

protected:
    GuiSystem::AbstractEditor *createEditor(QWidget *parent);
};

} // namespace HelloWorld

#endif // IMAGEVIEWEDITOR_H
