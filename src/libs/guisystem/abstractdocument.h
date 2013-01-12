#ifndef ABSTRACTDOCUMENT_H
#define ABSTRACTDOCUMENT_H

#include "guisystem_global.h"

#include <QtCore/QObject>
#include <QtCore/QUrl>

#include <QtGui/QIcon>

namespace GuiSystem {

class IHistory;

class AbstractDocumentPrivate;
class GUISYSTEM_EXPORT AbstractDocument : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(AbstractDocument)
    Q_DISABLE_COPY(AbstractDocument)

    Q_PROPERTY(QIcon icon READ icon NOTIFY iconChanged)
    Q_PROPERTY(bool modified READ isModified WRITE setModified NOTIFY modificationChanged)
    Q_PROPERTY(int progress READ progress NOTIFY progressChanged)
    Q_PROPERTY(State state READ state NOTIFY stateChanged)
    Q_PROPERTY(QString title READ title NOTIFY titleChanged)
    Q_PROPERTY(QUrl url READ url WRITE setUrl NOTIFY urlChanged)
    Q_PROPERTY(bool writable READ isWritable WRITE setWritable NOTIFY writableChanged)
public:
    enum State { NoState = 0,
                 OpenningState,
                 SavingState
               };
    Q_ENUMS(State)

    explicit AbstractDocument(QObject *parent = 0);
    ~AbstractDocument();

    QIcon icon() const;

    bool isModified() const;
    int progress() const;
    bool isWritable() const;

    State state() const;
    QString title() const;
    QUrl url() const;

    virtual IHistory *history() const;

public slots:
    virtual void clear();
    virtual void reload();

    virtual void setModified(bool modified = true);

    void setUrl(const QUrl &url);

    virtual void save(const QUrl &url);
    virtual void stop();

signals:
    void iconChanged(const QIcon &icon);

    void modificationChanged(bool modified);
    void progressChanged(int progress);

    void stateChanged(AbstractDocument::State state);

    void titleChanged(const QString &title);
    void urlChanged(const QUrl &url);
    void writableChanged(bool readOnly);

protected slots:
    void setIcon(const QIcon &icon);
    void setProgress(int progress);
    void setState(AbstractDocument::State state);
    void setTitle(const QString &title);
    void setWritable(bool writable = true);

protected:
    AbstractDocument(AbstractDocumentPrivate &dd, QObject *parent);

    virtual bool openUrl(const QUrl &url) = 0;

    AbstractDocumentPrivate *d_ptr;
};

} // namespace GuiSystem

#endif // ABSTRACTDOCUMENT_H
