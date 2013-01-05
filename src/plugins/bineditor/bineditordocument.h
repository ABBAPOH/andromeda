#ifndef BINEDITORDOCUMENT_H
#define BINEDITORDOCUMENT_H

#include "bineditor_global.h"

#include <guisystem/abstractdocument.h>

namespace BINEditor {

class BINEDITOR_EXPORT BinEditorDocument : public GuiSystem::AbstractDocument
{
    Q_OBJECT
public:
    explicit BinEditorDocument(QObject *parent = 0);

protected:
    bool openUrl(const QUrl &url);
};

} // namespace BINEditor

#endif // BINEDITORDOCUMENT_H
