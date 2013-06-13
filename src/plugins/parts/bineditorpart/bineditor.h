#ifndef BINEDITOR_H
#define BINEDITOR_H

#include "bineditor_global.h"

#include <Parts/AbstractEditor>
#include <Parts/AbstractEditorFactory>

class BinEdit;

namespace BINEditor {

class BinEditorDocument;
class BINEDITOR_EXPORT BinEditor : public Parts::AbstractEditor
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

    void setDocument(Parts::AbstractDocument *document);

protected:
    void resizeEvent(QResizeEvent *);

private:
    void createActions();
    void retranslateUi();

private slots:
    void open(const QUrl &url);

private:
    BinEdit *m_editor;

    QAction *actions[ActionCount];
};

class BinEditorFactory : public Parts::AbstractEditorFactory
{
public:
    explicit BinEditorFactory(QObject *parent = 0);

protected:
    Parts::AbstractDocument *createDocument(QObject *parent);
    Parts::AbstractEditor *createEditor(QWidget *parent);
};

} // namespace HelloWorld

#endif // BINEDITOR_H
