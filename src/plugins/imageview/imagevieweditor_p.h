#ifndef IMAGEVIEWEDITOR_P_H
#define IMAGEVIEWEDITOR_P_H

#include "imagevieweditor.h"

#include <guisystem/ifile.h>

namespace ImageView {

class ImageViewFile : public GuiSystem::IFile
{
public:
    explicit ImageViewFile(QObject *parent) : IFile(parent) {}
};

} // namespace ImageView

#endif // IMAGEVIEWEDITOR_P_H
