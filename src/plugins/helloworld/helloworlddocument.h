#ifndef HELLOWORLDDOCUMENT_H
#define HELLOWORLDDOCUMENT_H

#include "helloworld_global.h"

#include <GuiSystem/AbstractDocument>
#include <GuiSystem/AbstractDocumentFactory>

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

class HelloWorldDocumentFactory : public GuiSystem::AbstractDocumentFactory
{
    Q_OBJECT
    Q_DISABLE_COPY(HelloWorldDocumentFactory)
public:
    explicit HelloWorldDocumentFactory(QObject *parent = 0);

    QByteArray id() const;
    QString name() const;
    QIcon icon() const;

protected:
    GuiSystem::AbstractDocument *createDocument(QObject *parent);
};

} // namespace HelloWorld

#endif // HELLOWORLDDOCUMENT_H
