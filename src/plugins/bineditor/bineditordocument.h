#ifndef BINEDITORDOCUMENT_H
#define BINEDITORDOCUMENT_H

#include "bineditor_global.h"

#include <GuiSystem/AbstractDocument>
#include <GuiSystem/AbstractDocumentFactory>

namespace BINEditor {

class BINEDITOR_EXPORT BinEditorDocument : public GuiSystem::AbstractDocument
{
    Q_OBJECT
public:
    explicit BinEditorDocument(QObject *parent = 0);

protected:
    bool openUrl(const QUrl &url);
};

class BinEditorDocumentFactory : public GuiSystem::AbstractDocumentFactory
{
    Q_OBJECT
public:
    explicit BinEditorDocumentFactory(QObject *parent = 0);

    QByteArray id() const;
    QString name() const;
    QIcon icon() const;
    QStringList mimeTypes() const;
    int weight() const;

protected:
    GuiSystem::AbstractDocument *createDocument(QObject *parent);
};

} // namespace BINEditor

#endif // BINEDITORDOCUMENT_H
