#include "imageviewdocument.h"

#include <QtCore/QFile>
#include <QtCore/QFileInfo>

#include <qimageview/qimageview.h>

#include "imagevieweditor.h"

using namespace GuiSystem;
using namespace ImageView;

ImageViewDocument::ImageViewDocument(QObject *parent) :
    FileDocument(parent)
{
    setIcon(QIcon(":/icons/imageview.png"));
}

bool ImageViewDocument::read(QIODevice *device, const QString &fileName)
{
    editor->m_view->read(device, QFileInfo(fileName).suffix().toUtf8());
    return true;
}

bool ImageViewDocument::write(QIODevice *device, const QString &fileName)
{
    editor->m_view->write(device, QFileInfo(fileName).suffix().toUtf8());
    return true;
}
