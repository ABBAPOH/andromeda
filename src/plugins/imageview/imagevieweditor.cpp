#include "imagevieweditor.h"

#include <QtCore/QUrl>
#include <QtCore/QFileInfo>
#include <QtGui/QResizeEvent>

#include <qimageview/qimageview.h>

using namespace GuiSystem;
using namespace ImageView;

ImageViewEditor::ImageViewEditor(QWidget *parent) :
    AbstractEditor(parent),
    m_view(new QImageView(this))
{
}

void ImageViewEditor::open(const QUrl &url)
{
    if (m_url == url)
        return;

    m_url = url;
//    m_view->clear();
    QString path = url.toLocalFile();
    if (path.isEmpty()) {
        emit loadFinished(false);
        return;
    }

    QFile file(path);
    if (!file.open(QFile::ReadOnly)) {
        emit loadFinished(false);
        return;
    }

    m_view->read(&file, QFileInfo(path).suffix().toUtf8());

    emit urlChanged(m_url);
    emit iconChanged(icon());
    emit titleChanged(title());
    emit windowTitleChanged(windowTitle());
    emit loadFinished(true);
}

QUrl ImageViewEditor::url() const
{
    return m_url;
}

QIcon ImageViewEditor::icon() const
{
    return QIcon(":/icons/imageview.png");
}

QString ImageViewEditor::title() const
{
    QString path = m_url.path();
    return QFileInfo(path).fileName();
}

QString ImageViewEditor::windowTitle() const
{
    return title();
}

void ImageViewEditor::resizeEvent(QResizeEvent *e)
{
    m_view->resize(e->size());
}

ImageViewEditorFactory::ImageViewEditorFactory(QObject *parent) :
    AbstractEditorFactory(parent)
{
}

QByteArray ImageViewEditorFactory::id() const
{
    return "imageview";
}

QStringList ImageViewEditorFactory::mimeTypes() const
{
    return QStringList() << "image/jpeg" << "image/png";
}

AbstractEditor * ImageViewEditorFactory::createEditor(QWidget *parent)
{
    return new ImageViewEditor(parent);
}
