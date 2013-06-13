#ifndef TEXTFIND_H
#define TEXTFIND_H

#include <QtGui/QTextCursor>
#include <Parts/IFind>

class QTextDocument;

namespace TextEditor {

class TextFind : public Parts::IFind
{
    Q_OBJECT
public:
    explicit TextFind(QObject *parent = 0);

    bool supportsReplace() const;
    FindFlags supportedFindFlags() const;

    void clearResults();

    QString currentFindString() const;
    QString completedFindString() const;

    void findIncremental(const QString &text, FindFlags findFlags);
    void findStep(const QString &txt, FindFlags findFlags);
    void replace(const QString &before, const QString &after, FindFlags findFlags);
    bool replaceStep(const QString &before, const QString &after, FindFlags findFlags);
    int replaceAll(const QString &before, const QString &after, FindFlags findFlags);

    void setDocument(QTextDocument *document);

    QTextCursor textCursor() const;
    void setTextCursor(const QTextCursor &textCursor);

signals:
    void cursorChanged();

private:
    QString m_text;
    QTextDocument *m_document;
    QTextCursor m_cursor;
};

} // namespace TextEditor

#endif // TEXTFIND_H
