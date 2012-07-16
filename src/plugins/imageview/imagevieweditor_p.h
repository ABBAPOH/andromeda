#ifndef IMAGEVIEWEDITOR_P_H
#define IMAGEVIEWEDITOR_P_H

#include "imagevieweditor.h"

#include <guisystem/ifile.h>
#include <qimageview/qimageview.h>

namespace ImageView {

class ImageViewFile : public GuiSystem::IFile
{
public:
    explicit ImageViewFile(ImageViewEditor *parent) : IFile(parent), editor(parent) {}

    bool isModified() const { return editor->m_view->isModified(); }
    void setModified(bool modified) { editor->m_view->setModified(modified); }

    bool isReadOnly() const { return false; }

    void save(const QUrl &url);

private:
    ImageViewEditor *editor;
};

} // namespace ImageView

#endif // IMAGEVIEWEDITOR_P_H
