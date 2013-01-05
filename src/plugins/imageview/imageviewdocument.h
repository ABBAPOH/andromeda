#ifndef IMAGEVIEWDOCUMENT_H
#define IMAGEVIEWDOCUMENT_H

#include "imageview_global.h"
#include "imagevieweditor.h"

#include <guisystem/abstractdocument.h>
#include <qimageview/qimageview.h>

namespace ImageView {

class IMAGEVIEW_EXPORT ImageViewDocument : public GuiSystem::AbstractDocument
{
    Q_OBJECT
public:
    explicit ImageViewDocument(QObject *parent = 0);

    void setEditor(ImageViewEditor *editor) { this->editor = editor; }

    void save(const QUrl &url);

protected:
    bool openUrl(const QUrl &url);

private:
    ImageViewEditor *editor;
};

} // namespace ImageView

#endif // IMAGEVIEWDOCUMENT_H
