#ifndef ABSTRACTDOCUMENT_H
#define ABSTRACTDOCUMENT_H

#include "guisystem_global.h"

#include <QtCore/QObject>
#include <QtCore/QUrl>

#include <QtGui/QIcon>

namespace GuiSystem {

class AbstractDocumentPrivate;
class GUISYSTEM_EXPORT AbstractDocument : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(AbstractDocument)
    Q_DISABLE_COPY(AbstractDocument)

    Q_PROPERTY(QIcon icon READ icon NOTIFY iconChanged)
    Q_PROPERTY(bool modified READ isModified WRITE setModified NOTIFY modificationChanged)
    Q_PROPERTY(bool readOnly READ isReadOnly WRITE setReadOnly NOTIFY readOnlyChanged)
    Q_PROPERTY(QString title READ title NOTIFY titleChanged)
    Q_PROPERTY(QUrl url READ url WRITE setUrl NOTIFY urlChanged)

public:
    explicit AbstractDocument(QObject *parent = 0);
    ~AbstractDocument();

    QIcon icon() const;

    bool isModified() const;
    bool isReadOnly() const;

    QString title() const;
    QUrl url() const;

public slots:
    virtual void clear();
    virtual void reload();

    virtual void setModified(bool modified = true);
    virtual void setReadOnly(bool readOnly = true);

    void setUrl(const QUrl &url);

    virtual void save(const QUrl &url);
    virtual void stop();

signals:
    void iconChanged(const QIcon &icon);

    void modificationChanged(bool modified);
    void readOnlyChanged(bool readOnly);

    void titleChanged(const QString &title);
    void urlChanged(const QUrl &url);

    void loadStarted();
    void loadFinished(bool ok);
    void loadProgress(int progress);

protected:
    AbstractDocument(AbstractDocumentPrivate &dd, QObject *parent);

    void setIcon(const QIcon &icon);
    void setTitle(const QString &title);
    virtual bool openUrl(const QUrl &url) = 0;

    AbstractDocumentPrivate *d_ptr;
};

} // namespace GuiSystem

#endif // ABSTRACTDOCUMENT_H
