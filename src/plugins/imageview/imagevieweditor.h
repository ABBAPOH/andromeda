#ifndef IMAGEVIEWEDITOR_H
#define IMAGEVIEWEDITOR_H

#include "imageview_global.h"

#include <GuiSystem/AbstractEditor>
#include <GuiSystem/AbstractEditorFactory>

class QImageView;
class QToolBar;
class QVBoxLayout;

namespace ImageView {

class ImageViewDocument;

class IMAGEVIEW_EXPORT ImageViewEditor : public GuiSystem::AbstractEditor
{
    Q_OBJECT
    Q_DISABLE_COPY(ImageViewEditor)

public:
    explicit ImageViewEditor(QWidget *parent = 0);

    void setDocument(GuiSystem::AbstractDocument *document);

protected:
    void setupUi();

private:
    QVBoxLayout *m_layout;
    QToolBar *m_toolBar;
    QImageView *m_view;

    friend class ImageViewDocument;
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
    GuiSystem::AbstractDocument *createDocument(QObject *parent);
    GuiSystem::AbstractEditor *createEditor(QWidget *parent);
};

} // namespace HelloWorld

#endif // IMAGEVIEWEDITOR_H
