#ifndef IMAGEVIEWERDOCUMENT_H
#define IMAGEVIEWERDOCUMENT_H

#include "imageviewereditor.h"

#include <Parts/AbstractDocumentFactory>
#include <Parts/FileDocument>
#include <ImageView/ImageView>

namespace ImageViewer {

class ImageViewerDocument : public Parts::FileDocument
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

class ImageViewerDocumentFactory : public Parts::AbstractDocumentFactory
{
    Q_OBJECT
    Q_DISABLE_COPY(ImageViewerDocumentFactory)

public:
    explicit ImageViewerDocumentFactory(QObject *parent = 0);

    QString name() const;
    QIcon icon() const;
    QStringList mimeTypes() const;

protected:
    Parts::AbstractDocument *createDocument(QObject *parent);
};

} // namespace ImageViewer

#endif // IMAGEVIEWERDOCUMENT_H
