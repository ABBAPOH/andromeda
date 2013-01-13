#ifndef EDITORVIEWHISTORY_H
#define EDITORVIEWHISTORY_H

#include "ihistory.h"

namespace GuiSystem {

class AbstractEditor;
class EditorView;

class EditorViewHistoryPrivate;
class EditorViewHistory : public IHistory
{
    Q_OBJECT

public:
    explicit EditorViewHistory(QObject *parent = 0);
    ~EditorViewHistory();

    void setContainer(EditorView *container);

    void open(const QUrl &url, GuiSystem::AbstractEditor *oldEditor);

    int count() const;

    int currentItemIndex() const;
    void setCurrentItemIndex(int index);

    HistoryItem itemAt(int index) const;

    QByteArray store() const;
    void restore(const QByteArray &);

public slots:
    void clear();

private slots:
    void localHistoryIndexChanged(int index);
    void onUrlChanged(const QUrl &url);

private:
    EditorViewHistoryPrivate *d;
};

} // namespace GuiSystem

#endif // EDITORVIEWHISTORY_H
