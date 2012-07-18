#ifndef ABSTRACTEDITORCONTAINER_H
#define ABSTRACTEDITORCONTAINER_H

#include "guisystem_global.h"

#include "abstracteditor.h"

#include <QtCore/QPointer>

namespace GuiSystem {

class GUISYSTEM_EXPORT ProxyEditor : public AbstractEditor
{
    Q_OBJECT

public:
    explicit ProxyEditor(QWidget *parent = 0);

    AbstractEditor *sourceEditor() const;
    virtual void setSourceEditor(AbstractEditor *editor);

    QUrl url() const;

    QIcon icon() const;
    QImage preview() const;
    QString title() const;
    QString windowTitle() const;

    void open(const QUrl &url = QUrl());

    void cancel();
    void close();
    void refresh();

signals:
    void currentChanged(int index);

protected:
    void connectEditor(AbstractEditor *editor);
    void disconnectEditor(AbstractEditor *editor);

protected:
    QPointer<AbstractEditor> m_editor;
};

} // namespace GuiSystem

#endif // ABSTRACTEDITORCONTAINER_H
