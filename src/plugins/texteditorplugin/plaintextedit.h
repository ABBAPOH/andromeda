#ifndef PLAINTEXTEDIT_H
#define PLAINTEXTEDIT_H

#include <QtGui/QPlainTextEdit>

class QAction;

namespace TextEditor {

class PlainTextEdit : public QPlainTextEdit
{
    Q_OBJECT
    Q_DISABLE_COPY(PlainTextEdit)

public:
    explicit PlainTextEdit(QWidget *parent = 0);

    enum Action {
        NoAction = -1,
        New,
        Save,
        SaveAs,

        Undo,
        Redo,
        Copy,
        Cut,
        Paste,

        ZoomIn,
        ZoomOut,

        ActionsCount
    };

    QAction *action(Action action) const;

public slots:
    void zoomIn();
    void zoomOut();

private:
    void createActions();

private:
    QAction *actions[ActionsCount];
};

} // namespace TextEditor

#endif // PLAINTEXTEDIT_H
