#ifndef STACKEDCONTAINER_H
#define STACKEDCONTAINER_H

#include "guisystem_global.h"

#include "abstractcontainer.h"

namespace GuiSystem {

class StackedContainerPrivate;
class GUISYSTEM_EXPORT StackedContainer : public AbstractContainer
{
    Q_OBJECT
    Q_DISABLE_COPY(StackedContainer)

public:
    explicit StackedContainer(QWidget *parent = 0);
    ~StackedContainer();

    Capabilities capabilities() const;

    int count() const;
    int currentIndex() const;
    AbstractEditor *editor(int index) const;

    IFile *file() const;

    QUrl url() const;

    IHistory *history() const;

    virtual bool restoreState(const QByteArray &state);
    virtual QByteArray saveState() const;

public slots:
    void open(const QUrl &url = QUrl());
    void openNewEditor(const QUrl &url);

    void setCurrentIndex(int index);

private slots:
    void onUrlChanged(const QUrl &url);
    void onIndexChanged(int index);
    void onDestroy(QObject*);

private:
    StackedContainerPrivate *d;

    friend class StackedContainerPrivate;
};

} // namespace GuiSystem

#endif // STACKEDCONTAINER_H
