#ifndef HELLOWORLDDOCUMENT_H
#define HELLOWORLDDOCUMENT_H

#include <Parts/AbstractDocument>
#include <Parts/AbstractDocumentFactory>

namespace HelloWorld {

class HelloWorldDocument : public Parts::AbstractDocument
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

class HelloWorldDocumentFactory : public Parts::AbstractDocumentFactory
{
    Q_OBJECT
    Q_DISABLE_COPY(HelloWorldDocumentFactory)
public:
    explicit HelloWorldDocumentFactory(QObject *parent = 0);

    QString name() const;
    QIcon icon() const;

protected:
    Parts::AbstractDocument *createDocument(QObject *parent);
};

} // namespace HelloWorld

#endif // HELLOWORLDDOCUMENT_H
