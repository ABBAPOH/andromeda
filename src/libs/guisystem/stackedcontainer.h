#ifndef STACKEDCONTAINER_H
#define STACKEDCONTAINER_H

#include "guisystem_global.h"

#include "proxyeditor.h"

namespace GuiSystem {

class StackedContainerPrivate;
class GUISYSTEM_EXPORT StackedContainer : public ProxyEditor
{
    Q_OBJECT
    Q_DISABLE_COPY(StackedContainer)

public:
    explicit StackedContainer(QWidget *parent = 0);
    ~StackedContainer();

    void setSourceEditor(AbstractEditor *editor);

    AbstractEditor *currentEditor() const;

    IFile *file() const;

    QUrl url() const;

    IHistory *history() const;

    virtual bool restoreState(const QByteArray &state);
    virtual QByteArray saveState() const;

    AbstractEditor *editor(const QString &id) const;
    bool setEditor(const QString &id);

public slots:
    void open(const QUrl &url = QUrl());
    void openEditor(const QUrl &url, const QByteArray &editor);

private slots:
    void onUrlChanged(const QUrl &url);
    void onDestroy(QObject*);

private:
    StackedContainerPrivate *d;

    friend class StackedContainerPrivate;
};

} // namespace GuiSystem

#endif // STACKEDCONTAINER_H
