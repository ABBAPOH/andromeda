#include "textfind.h"

#include <QtGui/QTextDocument>

using namespace Parts;
using namespace TextEditor;

static QTextDocument::FindFlags iFind2TextDocumentFlags(IFind::FindFlags flags)
{
    QTextDocument::FindFlags result = 0;
    if (flags & IFind::FindBackward)
        result |= QTextDocument::FindBackward;
    if (flags & IFind::FindCaseSensitively)
        result |= QTextDocument::FindCaseSensitively;
    if (flags & IFind::FindWholeWords)
        result |= QTextDocument::FindWholeWords;

    return result;
}

TextFind::TextFind(QObject *parent) :
    IFind(parent),
    m_document(0)
{
}

bool TextFind::supportsReplace() const
{
    return true;
}

IFind::FindFlags TextFind::supportedFindFlags() const
{
    return IFind::FindFlags(FindBackward | FindCaseSensitively | FindWholeWords /*| FindRegularExpression*/);
}

void TextFind::clearResults()
{
    m_text.clear();
}

QString TextFind::currentFindString() const
{
    return m_text;
}

QString TextFind::completedFindString() const
{
    return currentFindString();
}

void TextFind::findIncremental(const QString &text, IFind::FindFlags findFlags)
{
    QTextDocument::FindFlags flags = iFind2TextDocumentFlags(findFlags);

    QTextCursor cursor = textCursor();
    if (cursor.hasSelection())
        cursor.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, cursor.position() - cursor.selectionStart());
    if (cursor.atEnd())
        cursor.movePosition(QTextCursor::Start);

    cursor = m_document->find(text, cursor, flags);
    if (cursor.isNull()) {
        // We try to wrap around text document
        cursor = QTextCursor(m_document);
        cursor.movePosition(QTextCursor::Start);
        cursor = m_document->find(text, cursor, flags);
    }

    if (!cursor.isNull())
        setTextCursor(cursor);
}

void TextFind::findStep(const QString &text, IFind::FindFlags findFlags)
{
    QTextDocument::FindFlags flags = iFind2TextDocumentFlags(findFlags);
    QTextCursor cursor = textCursor();

    cursor = m_document->find(text, cursor, flags);
    if (cursor.isNull()) {
        // We try to wrap around text document
        cursor = QTextCursor(m_document);
        cursor.movePosition(QTextCursor::Start);
        cursor = m_document->find(text, cursor, flags);
    }

    if (!cursor.isNull())
        setTextCursor(cursor);
}

void TextFind::replace(const QString &before, const QString &after, IFind::FindFlags /*findFlags*/)
{
    QTextCursor cursor = textCursor();
    if (cursor.hasSelection() && cursor.selectedText() == before ) {
        cursor.removeSelectedText();
        cursor.insertText(after);
    }
}

bool TextFind::replaceStep(const QString &before, const QString &after, IFind::FindFlags findFlags)
{
    replace(before, after, findFlags);
    findStep(before, findFlags);
    return true;
}

int TextFind::replaceAll(const QString &before, const QString &after, IFind::FindFlags findFlags)
{
    QTextDocument::FindFlags flags = iFind2TextDocumentFlags(findFlags);
    QTextCursor cursor = QTextCursor(m_document);
    cursor.movePosition(QTextCursor::Start);
    cursor = m_document->find(before, cursor, flags);

    int count = 0;
    while (!cursor.isNull()) {
        count++;
        if (count == 0)
            cursor.beginEditBlock();
        else
            cursor.joinPreviousEditBlock();
        cursor.removeSelectedText();
        cursor.insertText(after);
        cursor.endEditBlock();
        cursor = m_document->find(before, cursor, flags);
    }

    return count;
}

void TextFind::setDocument(QTextDocument *document)
{
    m_document = document;
}

QTextCursor TextFind::textCursor() const
{
    return m_cursor;
}

void TextFind::setTextCursor(const QTextCursor &cursor)
{
    if (m_cursor == cursor)
        return;

    m_cursor = cursor;
    emit cursorChanged();
}
