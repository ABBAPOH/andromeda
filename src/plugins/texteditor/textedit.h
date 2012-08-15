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

    void open(const QString &filePath);

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
    void saveFile(QIODevice *device);

protected:
    void resizeEvent(QResizeEvent *event);

private slots:
    void highlightCurrentLine();

public slots:
    void enlargeFont();
    void shrinkFont();
    void newFile();
    void loadSettings();

private:
    void createActions();
    QString strippedName(const QString &fullFileName);

private:
    QAction *actions[ActionsCount];
    QString curFile;
};

#endif // TEXTEDIT_H
