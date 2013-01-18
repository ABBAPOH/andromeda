#ifndef TEXTEDIT_H
#define TEXTEDIT_H

#include <QPlainTextEdit>
#include <QObject>
#include <QIODevice>
#include <QFont>

class TextEdit : public QPlainTextEdit
{
    Q_OBJECT
public:
    TextEdit(QWidget *parent = 0);

    enum Action {
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

#endif // TEXTEDIT_H
