#ifndef EDITORVIEW_H
#define EDITORVIEW_H

#include "guisystem_global.h"

#include <QtGui/QWidget>
#include "abstracteditor.h"

namespace GuiSystem {

class IHistory;

class EditorViewPrivate;
class GUISYSTEM_EXPORT EditorView : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(EditorView)

public:
    explicit EditorView(QWidget *parent = 0);
    ~EditorView();

    void setSourceEditor(AbstractEditor *editor);

    AbstractEditor *currentEditor() const;

    QUrl url() const;

    IHistory *history() const;

    bool restoreState(const QByteArray &state);
    QByteArray saveState() const;

    AbstractDocument *document() const;
    AbstractEditor *editor() const;

public slots:
    void open(const QUrl &url = QUrl());
    void openEditor(const QUrl &url, const QByteArray &editor);
    void openEditor(const QByteArray &editorId);

signals:
    void editorChanged();

private slots:
    void onUrlChanged(const QUrl &url);

private:
    EditorViewPrivate *d;

    friend class EditorViewPrivate;
};

} // namespace GuiSystem

#endif // EDITORVIEW_H
