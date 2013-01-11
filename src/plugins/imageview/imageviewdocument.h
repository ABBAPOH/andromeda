#ifndef IMAGEVIEWDOCUMENT_H
#define IMAGEVIEWDOCUMENT_H

#include "imageview_global.h"
#include "imagevieweditor.h"

#include <GuiSystem/FileDocument>
#include <QImageView/QImageView>

namespace ImageView {

class IMAGEVIEW_EXPORT ImageViewDocument : public GuiSystem::FileDocument
{
    Q_OBJECT
public:
    explicit ImageViewDocument(QObject *parent = 0);

    void setEditor(ImageViewEditor *editor) { this->editor = editor; }

protected:
    bool read(QIODevice *device, const QString &fileName);
    bool write(QIODevice *device, const QString &fileName);

private:
    ImageViewEditor *editor;
};

} // namespace ImageView

#endif // IMAGEVIEWDOCUMENT_H
