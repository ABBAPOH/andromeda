#include "bineditordocument.h"

#include <QtCore/QFileInfo>
#include <QtGui/QFileIconProvider>

using namespace GuiSystem;
using namespace BINEditor;

BinEditorDocument::BinEditorDocument(QObject *parent) :
    AbstractDocument(parent)
{
    setWritable(false);
}

bool BinEditorDocument::openUrl(const QUrl &url)
{
    QString localFile = url.toLocalFile();
    QFileInfo info = QFileInfo(localFile);

    setTitle(info.baseName());
    setIcon(QFileIconProvider().icon(info));

    return true;
}

/*!
    \class BinEditorDocumentFactory
*/

BinEditorDocumentFactory::BinEditorDocumentFactory(QObject *parent):
    AbstractDocumentFactory("bineditor", parent)
{
}

/*!
    \reimp
*/
QStringList BinEditorDocumentFactory::mimeTypes() const
{
    return QStringList() << "application/octet-stream";
}

/*!
    \reimp
*/
QString BinEditorDocumentFactory::name() const
{
    return tr("Binary document");
}

/*!
    \reimp
*/
QIcon BinEditorDocumentFactory::icon() const
{
    return QIcon();
}

/*!
    \reimp
*/
int BinEditorDocumentFactory::weight() const
{
    return 20;
}

/*!
    \reimp
*/
AbstractDocument * BinEditorDocumentFactory::createDocument(QObject *parent)
{
    return new BinEditorDocument(parent);
}
