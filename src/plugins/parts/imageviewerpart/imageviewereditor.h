#ifndef IMAGEVIEWEREDITOR_H
#define IMAGEVIEWEREDITOR_H

#include <Parts/AbstractEditor>
#include <Parts/AbstractEditorFactory>

class QToolBar;
class QVBoxLayout;

namespace ImageViewer {

class ImageView;
class ImageViewerDocument;

class ImageViewerEditor : public Parts::AbstractEditor
{
    Q_OBJECT
    Q_DISABLE_COPY(ImageViewerEditor)

public:
    explicit ImageViewerEditor(QWidget *parent = 0);

    void setDocument(Parts::AbstractDocument *document);

protected:
    void setupUi();

private:
    QVBoxLayout *m_layout;
    QToolBar *m_toolBar;
    ImageView *m_view;

    friend class ImageViewerDocument;
};

class ImageViewerEditorFactory : public Parts::AbstractEditorFactory
{
    Q_OBJECT
    Q_DISABLE_COPY(ImageViewerEditorFactory)

public:
    explicit ImageViewerEditorFactory(QObject *parent = 0);

protected:
    Parts::AbstractEditor *createEditor(QWidget *parent);
};

} // namespace ImageViewer

#endif // IMAGEVIEWEREDITOR_H
