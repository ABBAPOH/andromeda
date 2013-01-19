#ifndef PDFVIEWDOCUMENT_H
#define PDFVIEWDOCUMENT_H

#include "pdfview_global.h"

#include <GuiSystem/AbstractDocument>
#include <GuiSystem/AbstractDocumentFactory>

//namespace PdfView {

class PDFVIEW_EXPORT PdfViewDocument : public GuiSystem::AbstractDocument
{
    Q_OBJECT
public:
    explicit PdfViewDocument(QObject *parent = 0);

protected:
    bool openUrl(const QUrl &/*url*/) { return true; }
};

class PdfViewDocumentFactory : public GuiSystem::AbstractDocumentFactory
{
public:
    explicit PdfViewDocumentFactory(QObject *parent = 0);

    QString name() const;
    QIcon icon() const;
    QStringList mimeTypes() const;
    int weight() const;

protected:
    GuiSystem::AbstractDocument *createDocument(QObject *parent);
};

//} // namespace PdfView

#endif // PDFVIEWDOCUMENT_H
