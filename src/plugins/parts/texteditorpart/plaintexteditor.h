#ifndef PLAINTEXTEDITOR_H
#define PLAINTEXTEDITOR_H

#include <Parts/AbstractEditor>
#include <Parts/AbstractEditorFactory>

class QVBoxLayout;
class QToolBar;

namespace TextEditor {

class PlainTextEdit;
class TextFind;

class PlainTextDocument;
class PlainTextEditor : public Parts::AbstractEditor
{
    Q_OBJECT
    Q_DISABLE_COPY(PlainTextEditor)

public:
    explicit PlainTextEditor(QWidget *parent = 0);

    void setDocument(Parts::AbstractDocument *document);

    Parts::IFind *find() const;

private slots:
    void onCursorChanged();
    void onFindCursorChanged();

private:
    void setupUi();

    TextFind *m_find;
    PlainTextEdit *m_editor;
    QString m_currentFile;
};

class PlainTextEditorFactory : public Parts::AbstractEditorFactory
{
    Q_OBJECT
    Q_DISABLE_COPY(PlainTextEditorFactory)

public:
    explicit PlainTextEditorFactory(QObject *parent = 0);

protected:
    Parts::AbstractEditor *createEditor(QWidget *parent);
};

} // namespace TextEditor

#endif // PLAINTEXTEDITOR_H
