#ifndef IMAGEVIEWEREDITOR_H
#define IMAGEVIEWEREDITOR_H

#include "imageviewerplugin_global.h"

#include <GuiSystem/AbstractEditor>
#include <GuiSystem/AbstractEditorFactory>

class ImageView;
class QToolBar;
class QVBoxLayout;

namespace ImageViewer {

class ImageViewerDocument;

class IMAGEVIEWERPLUGIN_EXPORT ImageViewerEditor : public GuiSystem::AbstractEditor
{
    Q_OBJECT
    Q_DISABLE_COPY(ImageViewerEditor)
public:
    explicit ImageViewerEditor(QWidget *parent = 0);

    void setDocument(GuiSystem::AbstractDocument *document);

protected:
    void setupUi();

private:
    QVBoxLayout *m_layout;
    QToolBar *m_toolBar;
    ImageView *m_view;

    friend class ImageViewerDocument;
};

class ImageViewerEditorFactory : public GuiSystem::AbstractEditorFactory
{
public:
    explicit ImageViewerEditorFactory(QObject *parent = 0);

    QByteArray id() const;
    QString name() const;
    QIcon icon() const;
    QStringList mimeTypes() const;

protected:
    GuiSystem::AbstractDocument *createDocument(QObject *parent);
    GuiSystem::AbstractEditor *createEditor(QWidget *parent);
};

} // namespace ImageViewer

#endif // IMAGEVIEWEREDITOR_H
