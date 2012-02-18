#ifndef ABSTRACTEDITOR_H
#define ABSTRACTEDITOR_H

#include "abstractview.h"

#include <QtCore/QUrl>

namespace GuiSystem {

class AbstractHistory;
class AbstractEditorFactory;
class IFind;

class GUISYSTEM_EXPORT AbstractEditor : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(AbstractEditor)

    Q_PROPERTY(QByteArray id READ id)
    Q_PROPERTY(bool modified READ isModified WRITE setModified NOTIFY modificationChanged)
    Q_PROPERTY(bool readOnly READ isReadOnly WRITE setReadOnly NOTIFY readOnlyChanged)
    Q_PROPERTY(QUrl url READ url WRITE setUrl NOTIFY urlChanged)
    Q_PROPERTY(QIcon icon READ icon NOTIFY iconChanged)
    Q_PROPERTY(QString title READ title NOTIFY titleChanged)
    Q_PROPERTY(QString windowTitle READ windowTitle NOTIFY windowTitleChanged)

public:
    enum Capability { NoCapabilities = 0, CanSave = 0x1, HasHistory = 0x2, HasFind = 0x4 };
    Q_DECLARE_FLAGS(Capabilities, Capability)
    Q_FLAGS(Capabilities)

    explicit AbstractEditor(QWidget *parent = 0);
    ~AbstractEditor();

    QByteArray id() const;

    virtual Capabilities capabilities() const;

    virtual bool isModified() const;
    virtual bool isReadOnly() const;

    virtual QUrl url() const = 0;

    virtual QIcon icon() const;
    virtual QImage preview() const;
    virtual QString title() const;
    virtual QString windowTitle() const;

    virtual IFind *find() const;
    virtual AbstractHistory *history() const;

    virtual void restoreDefaults() {}
    virtual bool restoreState(const QByteArray &state);
    virtual QByteArray saveState() const;

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
    void capabilitiesChanged(Capabilities capabilities);

    void modificationChanged(bool modified);
    void readOnlyChanged(bool readOnly);
    void urlChanged(const QUrl &);

    void openTriggered(const QUrl &url);
    void openNewEditorTriggered(const QList<QUrl> &urls);
    void openNewWindowTriggered(const QList<QUrl> &urls);

    void iconChanged(const QIcon &icon);
    void titleChanged(const QString &title);
    void windowTitleChanged(const QString &title);

    void loadStarted();
    void loadFinished(bool ok);
    void loadProgress(int progress);

protected:
    AbstractEditorFactory *factory() const;
    void setFactory(AbstractEditorFactory *factory);

    ActionManager *actionManager() const;
    void addAction(QAction *action, const QByteArray &id);
    void registerAction(QAction *action, const QByteArray &id);

private:
    AbstractEditorFactory *m_factory;

    friend class AbstractEditorFactory;
};

} // namespace GuiSystem

#endif // ABSTRACTEDITOR_H
