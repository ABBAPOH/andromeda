#include "plaintextdocument.h"

#include <QtGui/QTextDocument>

#if QT_VERSION >= 0x050000
#include <QtWidgets/QPlainTextDocumentLayout>
#else
#include <QtGui/QPlainTextDocumentLayout>
#endif

using namespace Parts;
using namespace TextEditor;

/*!
    \class PlainTextDocument
*/

/*!
    Creates PlainTextDocument with the given \a parent.
*/
PlainTextDocument::PlainTextDocument(QObject *parent) :
    FileDocument(parent),
    m_textDocument(new QTextDocument(this))
{
    setIcon(QIcon(":/texteditor/icons/texteditor.png"));
    m_textDocument->setDocumentLayout(new QPlainTextDocumentLayout(m_textDocument));

    connect(m_textDocument, SIGNAL(modificationChanged(bool)), this, SLOT(setModified(bool)));
    connect(this, SIGNAL(modificationChanged(bool)), m_textDocument, SLOT(setModified(bool)));
}

QTextDocument * PlainTextDocument::textDocument() const
{
    return m_textDocument;
}

/*!
    \reimp
*/
bool PlainTextDocument::read(QIODevice *device, const QString &/*fileName*/)
{
    m_textDocument->setPlainText(QString::fromUtf8(device->readAll()));
    setModified(false);
    return true;
}

/*!
    \reimp
*/
bool PlainTextDocument::write(QIODevice *device, const QString &/*fileName*/)
{
    device->write(m_textDocument->toPlainText().toUtf8());
    return true;
}

/*!
    \class PlainTextDocumentFactory
*/

/*!
    Creates PlainTextDocumentFactory with the given \a parent.
*/
PlainTextDocumentFactory::PlainTextDocumentFactory(QObject *parent) :
    AbstractDocumentFactory("texteditor", parent)
{
}

/*!
    \reimp
*/
QString PlainTextDocumentFactory::name() const
{
    return tr("Text editor");
}

/*!
    \reimp
*/
QStringList PlainTextDocumentFactory::mimeTypes() const
{
    return QStringList() << "text/plain";
}

/*!
    \reimp
*/
QIcon PlainTextDocumentFactory::icon() const
{
    return QIcon(":/texteditor/icons/texteditor.png");
}

/*!
    \reimp
*/
AbstractDocument * PlainTextDocumentFactory::createDocument(QObject *parent)
{
    return new PlainTextDocument(parent);
}

