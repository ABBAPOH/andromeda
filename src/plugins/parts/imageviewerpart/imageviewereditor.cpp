#include "imageviewereditor.h"

#include <QtCore/QUrl>
#include <QtCore/QFileInfo>

#if QT_VERSION >= 0x050000
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#else
#include <QtGui/QToolBar>
#include <QtGui/QVBoxLayout>
#endif

#include <ImageView/ImageView>

#include "imageviewerdocument.h"

using namespace Parts;
using namespace ImageViewer;

ImageViewerEditor::ImageViewerEditor(QWidget *parent) :
    AbstractEditor(*new ImageViewerDocument, parent)
{
    document()->setParent(this);
    setupUi();

    connect(m_view, SIGNAL(modifiedChanged(bool)), document(), SLOT(setModified(bool)));
    connect(document(), SIGNAL(modificationChanged(bool)), m_view, SLOT(setModified(bool)));

    ImageViewerDocument *doc = static_cast<ImageViewerDocument *>(document());
    doc->setEditor(this);
}

void ImageViewerEditor::setDocument(AbstractDocument *document)
{
    ImageViewerDocument *imageDocument = qobject_cast<ImageViewerDocument*>(document);
    if (!imageDocument)
        return;

    imageDocument->setEditor(this);

    AbstractEditor::setDocument(document);
}

void ImageViewerEditor::setupUi()
{
    m_view = new ImageView(this);
    m_view->setFocusPolicy(Qt::StrongFocus);

    connect(m_view, SIGNAL(modifiedChanged(bool)), document(), SIGNAL(modificationChanged(bool)));

    m_toolBar = new QToolBar(this);
    m_toolBar->addAction(m_view->action(ImageView::ZoomIn));
    m_toolBar->addAction(m_view->action(ImageView::ZoomOut));
    m_toolBar->addSeparator();
    m_toolBar->addAction(m_view->action(ImageView::MoveTool));
    m_toolBar->addAction(m_view->action(ImageView::SelectionTool));
    m_toolBar->addSeparator();
    m_toolBar->addAction(m_view->action(ImageView::RotateLeft));
    m_toolBar->addAction(m_view->action(ImageView::RotateRight));

    m_layout = new QVBoxLayout(this);
    m_layout->setSpacing(0);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->addWidget(m_toolBar);
    m_layout->addWidget(m_view);
}

/*!
    \class ImageViewerEditorFactory
*/

/*!
    Creates ImageViewerEditorFactory with the given \a parent.
*/
ImageViewerEditorFactory::ImageViewerEditorFactory(QObject *parent) :
    AbstractEditorFactory("imageview", parent)
{
}

/*!
    \reimp
*/
AbstractEditor * ImageViewerEditorFactory::createEditor(QWidget *parent)
{
    return new ImageViewerEditor(parent);
}
