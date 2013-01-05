#ifndef BINEDITOR_H
#define BINEDITOR_H

#include "bineditor_global.h"

#include <guisystem/abstracteditor.h>
#include <guisystem/abstracteditorfactory.h>

class BinEdit;

namespace BINEditor {

class BinEditorDocument;
class BINEDITOR_EXPORT BinEditor : public GuiSystem::AbstractEditor
{
    Q_OBJECT
    Q_DISABLE_COPY(BinEditor)

public:
    enum Actions {
        Redo,
        Undo,

        Copy,
        SelectAll,

        ActionCount
    };

    explicit BinEditor(QWidget *parent = 0);

    void setDocument(GuiSystem::AbstractDocument *document);

protected:
    void resizeEvent(QResizeEvent *);

private:
    void createActions();
    void retranslateUi();
    void registerActions();

private slots:
    void open(const QUrl &url);

private:
    BinEdit *m_editor;

    QAction *actions[ActionCount];
};

class BinEditorFactory : public GuiSystem::AbstractEditorFactory
{
public:
    explicit BinEditorFactory(QObject *parent = 0);

    QByteArray id() const;
    QString name() const;
    QIcon icon() const;
    QStringList mimeTypes() const;
    int weight() const;

protected:
    GuiSystem::AbstractDocument *createDocument(QObject *parent);
    GuiSystem::AbstractEditor *createEditor(QWidget *parent);
};

} // namespace HelloWorld

#endif // BINEDITOR_H
