#ifndef TEXTEDITOR_H
#define TEXTEDITOR_H

#include "texteditor_global.h"
#include "textedit.h"

#include <guisystem/abstracteditor.h>
#include <guisystem/abstracteditorfactory.h>

#include <QToolBar>
#include <QVBoxLayout>


namespace TextEditor {

class TextEditorDocument;
class TEXTEDITOR_EXPORT TextEditorEditor : public GuiSystem::AbstractEditor
{
    Q_OBJECT
    Q_DISABLE_COPY(TextEditorEditor)
public:
    explicit TextEditorEditor(QWidget *parent = 0);

    void setDocument(GuiSystem::AbstractDocument *document);

    void reload();
    QUrl url() const;

    QIcon icon() const;
public slots:
    bool open(QIODevice *device);
    bool save(QIODevice *device, const QByteArray &format);

private:
    void setupUi();
    QUrl m_url;
    QVBoxLayout *m_layout;
    TextEdit *m_editor;
    QToolBar *m_toolBar;
    QString m_currentFile;

    friend class TextEditorDocument;
};

class TextEditorFactory : public GuiSystem::AbstractEditorFactory
{
public:
    explicit TextEditorFactory(QObject *parent = 0);

    QByteArray id() const;
    QString name() const;
    QStringList mimeTypes() const;
    QIcon icon() const;

protected:
    GuiSystem::AbstractDocument *createDocument(QObject *parent);
    GuiSystem::AbstractEditor *createEditor(QWidget *parent);
};

} // namespace TextEditor

#endif // TEXTEDITOR_H
