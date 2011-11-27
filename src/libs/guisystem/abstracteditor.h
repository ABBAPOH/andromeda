#ifndef ABSTRACTEDITOR_H
#define ABSTRACTEDITOR_H

#include "abstractview.h"

#include <QtCore/QUrl>

namespace GuiSystem {

class AbstractHistory;
class AbstractEditorFactory;
class IFind;

class GUISYSTEM_EXPORT AbstractEditor : public AbstractView
{
    Q_OBJECT
    Q_DISABLE_COPY(AbstractEditor)

    Q_PROPERTY(bool modified READ isModified WRITE setModified NOTIFY modificationChanged)
    Q_PROPERTY(bool readOnly READ isReadOnly WRITE setReadOnly NOTIFY readOnlyChanged)
    Q_PROPERTY(QUrl url READ url WRITE setUrl NOTIFY urlChanged)
    Q_PROPERTY(QString windowTitle READ windowTitle NOTIFY windowTitleChanged)

public:
    enum Capabilities { NoCapabilities = 0, CanSave = 0x1, HasHistory = 0x2, HasFind = 0x4 };

    explicit AbstractEditor(QWidget *parent = 0);
    ~AbstractEditor();

    virtual Capabilities capabilities() const;

    virtual bool isModified() const;
    virtual bool isReadOnly() const;

    virtual QUrl url() const = 0;

    virtual QImage preview() const;
    virtual QString windowTitle() const;

    virtual IFind *find() const;
    virtual AbstractHistory *history() const;

    virtual void restoreDefaults() {}
    void restoreState(const QByteArray &state);
    QByteArray saveState() const;

public slots:
    virtual void setModified(bool modified = true);
    virtual void setReadOnly(bool readOnly = true);

    virtual void open(const QUrl &url) = 0;
    void setUrl(const QUrl &url);

    virtual void cancel();
    virtual void close();
    virtual void refresh();

    virtual void save(const QUrl &url = QUrl());

signals:
    void modificationChanged(bool modified);
    void readOnlyChanged(bool readOnly);
    void urlChanged(const QUrl &);

    void openTriggered(const QUrl &url);
    void openNewEditorTriggered(const QList<QUrl> &urls);
    void openNewWindowTriggered(const QList<QUrl> &urls);

    void windowTitleChanged(const QString &title);

    void loadStarted();
    void loadFinished(bool ok);
    void loadProgress(int progress);

protected:
    AbstractEditorFactory *factory() const;

    friend class AbstractEditorFactory;
};

} // namespace GuiSystem

#endif // ABSTRACTEDITOR_H
