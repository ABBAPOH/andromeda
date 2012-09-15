#include "imagevieweditor.h"
#include "imagevieweditor_p.h"

#include <QtCore/QUrl>
#include <QtCore/QFileInfo>

#include <QtGui/QToolBar>
#include <QtGui/QVBoxLayout>

#include <qimageview/qimageview.h>

using namespace GuiSystem;
using namespace ImageView;

void ImageViewFile::save(const QUrl &url)
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

ImageViewEditor::ImageViewEditor(QWidget *parent) :
    AbstractEditor(parent),
    m_file(new ImageViewFile(this))
{
    setupUi();
    registerActions();
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

IFile *ImageViewEditor::file() const
{
    return m_file;
}

void ImageViewEditor::setupUi()
{
    m_view = new QImageView(this);
    m_view->setFocusPolicy(Qt::StrongFocus);

    connect(m_view, SIGNAL(modifiedChanged(bool)), m_file, SIGNAL(modificationChanged(bool)));

    m_toolBar = new QToolBar(this);
    m_toolBar->addAction(m_view->action(QImageView::ZoomIn));
    m_toolBar->addAction(m_view->action(QImageView::ZoomOut));
    m_toolBar->addSeparator();
    m_toolBar->addAction(m_view->action(QImageView::MoveTool));
    m_toolBar->addAction(m_view->action(QImageView::SelectionTool));
    m_toolBar->addSeparator();
    m_toolBar->addAction(m_view->action(QImageView::RotateLeft));
    m_toolBar->addAction(m_view->action(QImageView::RotateRight));

    m_layout = new QVBoxLayout(this);
    m_layout->setSpacing(0);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->addWidget(m_toolBar);
    m_layout->addWidget(m_view);
}

void ImageViewEditor::registerActions()
{
    registerAction(m_view->action(QImageView::Redo), "Redo");
    registerAction(m_view->action(QImageView::Undo), "Undo");
    registerAction(m_view->action(QImageView::Copy), "Copy");
//    registerAction(m_view->action(QImageView::Paste), "Paste");
    registerAction(m_view->action(QImageView::MoveTool), "MoveTool");
    registerAction(m_view->action(QImageView::SelectionTool), "SelectionTool");

    registerAction(m_view->action(QImageView::ZoomIn), "ZoomIn");
    registerAction(m_view->action(QImageView::ZoomOut), "ZoomOut");
    registerAction(m_view->action(QImageView::FitInView), "FitInView");
    registerAction(m_view->action(QImageView::NormalSize), "NormalSize");

    registerAction(m_view->action(QImageView::RotateLeft), "RotateLeft");
    registerAction(m_view->action(QImageView::RotateRight), "RotateRight");
    registerAction(m_view->action(QImageView::FlipHorizontally), "FlipHorizontally");
    registerAction(m_view->action(QImageView::FlipVertically), "FlipVertically");
    registerAction(m_view->action(QImageView::ResetOriginal), "ResetOriginal");
}

ImageViewEditorFactory::ImageViewEditorFactory(QObject *parent) :
    AbstractEditorFactory(parent)
{
}

QByteArray ImageViewEditorFactory::id() const
{
    return "imageview";
}

QString ImageViewEditorFactory::name() const
{
    return tr("Image viewer");
}

QIcon ImageViewEditorFactory::icon() const
{
    return QIcon(":/icons/imageview.png");
}

QStringList ImageViewEditorFactory::mimeTypes() const
{
    return QStringList() << "image/jpeg" << "image/png";
}

AbstractEditor * ImageViewEditorFactory::createEditor(QWidget *parent)
{
    return new ImageViewEditor(parent);
}
