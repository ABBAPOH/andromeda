#ifndef EDITORVIEW_H
#define EDITORVIEW_H

#include "../coreplugin_global.h"

#include <QtGui/QWidget>

class QUrl;
namespace CorePlugin {

class History;
class EditorViewPrivate;
class COREPLUGIN_EXPORT EditorView : public QWidget
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(EditorView)
    Q_DISABLE_COPY(EditorView)

public:
    explicit EditorView(QWidget *parent = 0);
    ~EditorView();

    History *history();

signals:

public slots:
    bool open(const QString &path, bool addToHistory = true);
    void close();

    void back();
    void forward();

private slots:
    void onCurrentItemIndexChanged(int);

protected:
    EditorViewPrivate *d_ptr;

    void resizeEvent(QResizeEvent *);
};

} // namespace CorePlugin

#endif // EDITORVIEW_H
