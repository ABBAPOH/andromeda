#ifndef TEXTEDIT_H
#define TEXTEDIT_H

#include <QPlainTextEdit>
#include <QObject>
#include <QIODevice>

QT_BEGIN_NAMESPACE
class QPaintEvent;
class QResizeEvent;
class QSize;
class QWidget;
QT_END_NAMESPACE

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
    QString currentFileName();

protected:
    void resizeEvent(QResizeEvent *event);

private slots:
    void highlightCurrentLine();

public slots:
    void enlargeFont();
    void shrinkFont();
    void loadSettings();

private:
    void createActions();

private:
    QAction *actions[ActionsCount];
};

#endif // TEXTEDIT_H
