#ifndef PLAINTEXTDOCUMENT_H
#define PLAINTEXTDOCUMENT_H

#include <Parts/AbstractDocumentFactory>
#include <Parts/FileDocument>

class QTextDocument;

namespace TextEditor {

class PlainTextDocument : public Parts::FileDocument
{
    Q_OBJECT
    Q_DISABLE_COPY(PlainTextDocument)

public:
    explicit PlainTextDocument(QObject *parent = 0);

    QTextDocument *textDocument() const;

protected:
    bool read(QIODevice *device, const QString &fileName);
    bool write(QIODevice *device, const QString &fileName);

protected:
    QTextDocument *m_textDocument;
};

class PlainTextDocumentFactory : public Parts::AbstractDocumentFactory
{
    Q_OBJECT
    Q_DISABLE_COPY(PlainTextDocumentFactory)

public:
    explicit PlainTextDocumentFactory(QObject *parent = 0);

    QString name() const;
    QStringList mimeTypes() const;
    QIcon icon() const;

protected:
    Parts::AbstractDocument *createDocument(QObject *parent);
};

} // namespace TextEditor

#endif // PLAINTEXTDOCUMENT_H
