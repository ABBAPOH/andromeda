#ifndef FILEDOCUMENT_P_H
#define FILEDOCUMENT_P_H

#include "abstractdocument_p.h"

#include "filedocument.h"

class QFile;

namespace GuiSystem {

class FileDocumentPrivate : public AbstractDocumentPrivate
{
    Q_DECLARE_PUBLIC(FileDocument)
    Q_DISABLE_COPY(FileDocumentPrivate)
public:
    explicit FileDocumentPrivate(FileDocument *qq) :
        AbstractDocumentPrivate(qq),
        file(0)
    {
    }

    QFile *file;
};

} // namespace GuiSystem

#endif // FILEDOCUMENT_P_H
