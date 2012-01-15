#ifndef ABSTRACTEDITORCONTAINER_H
#define ABSTRACTEDITORCONTAINER_H

#include "abstracteditor.h"

namespace GuiSystem {

class AbstractContainer : public AbstractEditor
{
    Q_OBJECT

    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentChanged)

public:
    explicit AbstractContainer(QWidget *parent = 0);

    Capabilities capabilities() const;

    virtual int count() const = 0;
    virtual int currentIndex() const = 0;
    virtual AbstractEditor *editor(int index) const = 0;

    AbstractEditor *currentEditor() const;

    bool isModified() const;
    bool isReadOnly() const;

    QUrl url() const;
    QList<QUrl> urls() const;

    QIcon icon() const;
    QImage preview() const;
    QString title() const;
    QString windowTitle() const;

public slots:
    void setModified(bool modified = true);
    void setReadOnly(bool readOnly = true);

    void open(const QUrl &url = QUrl());
    virtual void openNewEditor(const QUrl &url) = 0;
    void openNewEditor(const QList<QUrl> &urls);

    virtual void closeEditor(int /*index*/) {}

    void cancel();
    void close();
    void refresh();

    virtual void setCurrentIndex(int index) = 0;

    void save(const QUrl &url = QUrl());

signals:
    void currentChanged(int index);
};

} // namespace GuiSystem

#endif // ABSTRACTEDITORCONTAINER_H
