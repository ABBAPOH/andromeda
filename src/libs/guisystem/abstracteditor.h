#ifndef ABSTRACTEDITOR_H
#define ABSTRACTEDITOR_H

#include "abstractview.h"

#include <QtCore/QUrl>

namespace GuiSystem {

class IFile;
class IFind;
class IHistory;

class GUISYSTEM_EXPORT AbstractEditor : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(AbstractEditor)

    Q_PROPERTY(QUrl url READ url WRITE setUrl NOTIFY urlChanged)
    Q_PROPERTY(QIcon icon READ icon NOTIFY iconChanged)
    Q_PROPERTY(QString title READ title NOTIFY titleChanged)
public:
    explicit AbstractEditor(QWidget *parent = 0);
    ~AbstractEditor();

    virtual QUrl url() const = 0;

    virtual QIcon icon() const;
    virtual QImage preview() const;
    virtual QString title() const;

    virtual IFile *file() const;
    virtual IFind *find() const;
    virtual IHistory *history() const;

    virtual void restoreDefaults() {}
    virtual bool restoreState(const QByteArray &state);
    virtual QByteArray saveState() const;

public slots:
    virtual void open(const QUrl &url) = 0;
    void setUrl(const QUrl &url);

    virtual void cancel();
    virtual void close();
    virtual void refresh();
    virtual void clear();

signals:
    void urlChanged(const QUrl &);

    void openTriggered(const QUrl &url);

    void iconChanged(const QIcon &icon);
    void titleChanged(const QString &title);

    void loadStarted();
    void loadFinished(bool ok);
    void loadProgress(int progress);

protected:
    ActionManager *actionManager() const;
    void addAction(QAction *action, const QByteArray &id);
    void registerAction(QAction *action, const QByteArray &id);
};

} // namespace GuiSystem

#endif // ABSTRACTEDITOR_H
