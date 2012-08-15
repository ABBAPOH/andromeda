#ifndef TEXTEDITOR_H
#define TEXTEDITOR_H

#include <QToolBar>
#include <QVBoxLayout>
#include "texteditor_global.h"
#include "textedit.h"

#include <guisystem/abstracteditor.h>
#include <guisystem/abstracteditorfactory.h>

class QLabel;

namespace TEXTEditor {

class TextEditorFile;

class TEXTEDITOR_EXPORT TextEditor : public GuiSystem::AbstractEditor
{
    Q_OBJECT
    Q_DISABLE_COPY(TextEditor)

public:
    explicit TextEditor(QWidget *parent = 0);

    void open(const QUrl & url);
    QUrl url() const;

    QIcon icon() const;
    QString title() const;
    QString windowTitle() const;

    GuiSystem::IFile *file() const;

protected:
    void setupUi();

private:
    QUrl m_url;
    QVBoxLayout *m_layout;
    TextEdit *m_editor;
    QToolBar *m_toolBar;
    TextEditorFile *m_file;

    friend class TextEditorFile;

};

class TextEditorFactory : public GuiSystem::AbstractEditorFactory
{
public:
    explicit TextEditorFactory(QObject *parent = 0);

    QByteArray id() const;
    QStringList mimeTypes() const;

protected:
    GuiSystem::AbstractEditor *createEditor(QWidget *parent);
};

} // namespace TextEditor

#endif // TEXTEDITOR_H
