#include "pdfviewdocument.h"

using namespace GuiSystem;
//using namespace PdfView;

PdfViewDocument::PdfViewDocument(QObject *parent) :
    AbstractDocument(parent)
{
}


PdfViewDocumentFactory::PdfViewDocumentFactory(QObject *parent) :
    AbstractDocumentFactory("PdfView", parent)
{
}

QString PdfViewDocumentFactory::name() const
{
    return tr("PDF Viewer");
}

QIcon PdfViewDocumentFactory::icon() const
{
    return QIcon(":/icons/pdfview.png");
}

QStringList PdfViewDocumentFactory::mimeTypes() const
{
    return QStringList() << "application/pdf";
}

int PdfViewDocumentFactory::weight() const
{
    return 21;
}

AbstractDocument * PdfViewDocumentFactory::createDocument(QObject *parent)
{
    return new PdfViewDocument(parent);
}
