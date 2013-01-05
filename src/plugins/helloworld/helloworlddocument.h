#ifndef HELLOWORLDDOCUMENT_H
#define HELLOWORLDDOCUMENT_H

#include "helloworld_global.h"

#include <guisystem/abstractdocument.h>

namespace HelloWorld {

class HELLOWORLD_EXPORT HelloWorldDocument : public GuiSystem::AbstractDocument
{
    Q_OBJECT
public:
    explicit HelloWorldDocument(QObject *parent = 0);

protected:
    bool openUrl(const QUrl &url) { return true; }
};

} // namespace HelloWorld

#endif // HELLOWORLDDOCUMENT_H