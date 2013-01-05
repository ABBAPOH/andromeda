#include "imageviewdocument.h"

#include <QtCore/QFile>
#include <QtCore/QFileInfo>

#include <qimageview/qimageview.h>

#include "imagevieweditor.h"

using namespace GuiSystem;
using namespace ImageView;

ImageViewDocument::ImageViewDocument(QObject *parent) :
    AbstractDocument(parent)
{
    setIcon(QIcon(":/icons/imageview.png"));
}

void ImageViewDocument::save(const QUrl &url)
{
#if (QT_VERSION >= QT_VERSION_CHECK(4, 8, 0))
    if (!url.isLocalFile())
#else
    if (url.scheme() != QLatin1String("file"))
#endif
        return;

    QString path = url.toLocalFile();
    QFile file(path);
    if (!file.open(QFile::WriteOnly))
        return;

    editor->m_view->write(&file, QFileInfo(path).suffix().toUtf8());
}

bool ImageViewDocument::openUrl(const QUrl &url)
{
    QString path = url.path();
    if (path.isEmpty())
        return false;

    QFileInfo info(path);
    setTitle(info.fileName());

    QFile file(path);
    if (!file.open(QFile::ReadOnly))
        return false;

    editor->m_view->read(&file, QFileInfo(path).suffix().toUtf8());
    return true;
}
