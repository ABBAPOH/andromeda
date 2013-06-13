#ifndef BINEDITORDOCUMENT_H
#define BINEDITORDOCUMENT_H

#include "bineditor_global.h"

#include <Parts/AbstractDocument>
#include <Parts/AbstractDocumentFactory>

namespace BINEditor {

class BINEDITOR_EXPORT BinEditorDocument : public Parts::AbstractDocument
{
    Q_OBJECT
public:
    explicit BinEditorDocument(QObject *parent = 0);

protected:
    bool openUrl(const QUrl &url);
};

class BinEditorDocumentFactory : public Parts::AbstractDocumentFactory
{
    Q_OBJECT
public:
    explicit BinEditorDocumentFactory(QObject *parent = 0);

    QString name() const;
    QIcon icon() const;
    QStringList mimeTypes() const;
    int weight() const;

protected:
    Parts::AbstractDocument *createDocument(QObject *parent);
};

} // namespace BINEditor

#endif // BINEDITORDOCUMENT_H
