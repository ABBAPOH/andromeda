#ifndef PLAINTEXTDOCUMENT_H
#define PLAINTEXTDOCUMENT_H

#include "texteditor_global.h"

#include <GuiSystem/AbstractDocumentFactory>
#include <GuiSystem/FileDocument>

class QTextDocument;

namespace TextEditor {

class TEXTEDITOR_EXPORT PlainTextDocument : public GuiSystem::FileDocument
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

class PlainTextDocumentFactory : public GuiSystem::AbstractDocumentFactory
{
    Q_OBJECT
    Q_DISABLE_COPY(PlainTextDocumentFactory)

public:
    explicit PlainTextDocumentFactory(QObject *parent = 0);

    QString name() const;
    QStringList mimeTypes() const;
    QIcon icon() const;

protected:
    GuiSystem::AbstractDocument *createDocument(QObject *parent);
};

} // namespace TextEditor

#endif // PLAINTEXTDOCUMENT_H
