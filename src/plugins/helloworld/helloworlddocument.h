#ifndef HELLOWORLDDOCUMENT_H
#define HELLOWORLDDOCUMENT_H

#include "helloworld_global.h"

#include <guisystem/abstractdocument.h>

namespace HelloWorld {

class HELLOWORLD_EXPORT HelloWorldDocument : public GuiSystem::AbstractDocument
{
    Q_OBJECT
    Q_DISABLE_COPY(HelloWorldDocument)
public:
    explicit HelloWorldDocument(QObject *parent = 0);
    ~HelloWorldDocument();

public slots:
    void setTitle(const QString &title);

protected:
    bool openUrl(const QUrl &url);
};

} // namespace HelloWorld

#endif // HELLOWORLDDOCUMENT_H
