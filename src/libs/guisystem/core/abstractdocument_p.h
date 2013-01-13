#ifndef ABSTRACTDOCUMENT_P_H
#define ABSTRACTDOCUMENT_P_H

#include "abstractdocument.h"

namespace GuiSystem {

class AbstractDocumentPrivate
{
    Q_DECLARE_PUBLIC(AbstractDocument)
    Q_DISABLE_COPY(AbstractDocumentPrivate)
public:
    explicit AbstractDocumentPrivate(AbstractDocument *qq) :
        modified(false),
        progress(0),
        writable(true),
        state(AbstractDocument::NoState),
        q_ptr(qq)
    {
    }

    virtual ~AbstractDocumentPrivate()
    {
    }

    QIcon icon;
    bool modified;
    int progress;
    bool writable;
    AbstractDocument::State state;
    QString title;
    QUrl url;

protected:
    AbstractDocument *q_ptr;
};

} // namespace GuiSystem

#endif // ABSTRACTDOCUMENT_P_H
