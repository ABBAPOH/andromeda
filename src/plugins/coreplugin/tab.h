#ifndef TAB_H
#define TAB_H

#include "coreplugin_global.h"

#include <QtCore/QUrl>
#include <QtGui/QWidget>

#include "history.h"

class QSettings;

namespace CorePlugin {

class AbstractEditor;

class TabPrivate;
class COREPLUGIN_EXPORT Tab : public QWidget
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(Tab)
    Q_DISABLE_COPY(Tab)

    Q_PROPERTY(QUrl currentUrl READ currentUrl WRITE open NOTIFY currentUrlChanged)

public:
    explicit Tab(QWidget *parent = 0);
    ~Tab();

    QUrl currentUrl() const;
    QIcon icon() const;
    QString title() const;
    QString windowTitle() const;

    AbstractEditor *currentEditor() const;
    History *history() const;

    void restoreSession(QSettings &s);
    void saveSession(QSettings &s);

public slots:
    void open(const QUrl &url);

    void up();

signals:
    void currentUrlChanged(const QUrl &url);
    void changed();

private slots:
    void onIndexChanged(int index);
    void onUrlChanged(const QUrl &url);

protected:
//    void resizeEvent(QResizeEvent *);

protected:
    TabPrivate *d_ptr;
};

} // namespace CorePlugin

#endif // TAB_H
