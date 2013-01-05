#include "imagevieweditor.h"

#include <QtCore/QUrl>
#include <QtCore/QFileInfo>

#include <QtGui/QToolBar>
#include <QtGui/QVBoxLayout>

#include <qimageview/qimageview.h>

#include "imageviewdocument.h"

using namespace GuiSystem;
using namespace ImageView;

ImageViewEditor::ImageViewEditor(QWidget *parent) :
    AbstractEditor(*new ImageViewDocument, parent)
{
    document()->setParent(this);
    setupUi();
    registerActions();

    connect(m_view, SIGNAL(modifiedChanged(bool)), document(), SLOT(setModified(bool)));
    connect(document(), SIGNAL(modifiedChanged(bool)), m_view, SLOT(setModified(bool)));

    ImageViewDocument *doc = static_cast<ImageViewDocument *>(document());
    doc->setEditor(this);
}

void ImageViewEditor::setDocument(AbstractDocument *document)
{
    ImageViewDocument *imageDocument = qobject_cast<ImageViewDocument*>(document);
    if (!imageDocument)
        return;

    imageDocument->setEditor(this);

    AbstractEditor::setDocument(document);
}

void ImageViewEditor::setupUi()
{
    m_view = new QImageView(this);
    m_view->setFocusPolicy(Qt::StrongFocus);

    connect(m_view, SIGNAL(modifiedChanged(bool)), document(), SIGNAL(modificationChanged(bool)));

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

AbstractDocument * ImageViewEditorFactory::createDocument(QObject *parent)
{
    return new ImageViewDocument(parent);
}

AbstractEditor * ImageViewEditorFactory::createEditor(QWidget *parent)
{
    return new ImageViewEditor(parent);
}
