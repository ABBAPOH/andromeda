#ifndef PDFVIEWDOCUMENT_H
#define PDFVIEWDOCUMENT_H

#include "pdfview_global.h"

#include <guisystem/abstractdocument.h>

//namespace PdfView {

class PDFVIEW_EXPORT PdfViewDocument : public GuiSystem::AbstractDocument
{
    Q_OBJECT
public:
    explicit PdfViewDocument(QObject *parent = 0);

protected:
    bool openUrl(const QUrl &/*url*/) { return true; }
};

//} // namespace PdfView

#endif // PDFVIEWDOCUMENT_H
