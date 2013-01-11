#ifndef IMAGEVIEWERDOCUMENT_H
#define IMAGEVIEWERDOCUMENT_H

#include "imageviewerplugin_global.h"
#include "imageviewereditor.h"

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

} // namespace ImageViewer

#endif // IMAGEVIEWERDOCUMENT_H
