#ifndef IMAGEVIEWERDOCUMENT_H
#define IMAGEVIEWERDOCUMENT_H

#include "imageviewerplugin_global.h"
#include "imageviewereditor.h"

#include <GuiSystem/AbstractDocumentFactory>
#include <GuiSystem/FileDocument>
#include <ImageView/ImageView>

namespace ImageViewer {

class IMAGEVIEWERPLUGIN_EXPORT ImageViewerDocument : public GuiSystem::FileDocument
{
    Q_OBJECT
    Q_DISABLE_COPY(ImageViewerDocument)

public:
    explicit ImageViewerDocument(QObject *parent = 0);

    void setEditor(ImageViewerEditor *editor) { this->editor = editor; }

protected:
    bool read(QIODevice *device, const QString &fileName);
    bool write(QIODevice *device, const QString &fileName);

private:
    ImageViewerEditor *editor;
};

class ImageViewerDocumentFactory : public GuiSystem::AbstractDocumentFactory
{
    Q_OBJECT
    Q_DISABLE_COPY(ImageViewerDocumentFactory)

public:
    explicit ImageViewerDocumentFactory(QObject *parent = 0);

    QByteArray id() const;
    QString name() const;
    QIcon icon() const;
    QStringList mimeTypes() const;

protected:
    GuiSystem::AbstractDocument *createDocument(QObject *parent);
};

} // namespace ImageViewer

#endif // IMAGEVIEWERDOCUMENT_H
