#ifndef HELLOWORLDEDITOR_H
#define HELLOWORLDEDITOR_H

#include "helloworld_global.h"

#include <guisystem/abstracteditor.h>
#include <guisystem/abstracteditorfactory.h>

class QLabel;
class HELLOWORLD_EXPORT HelloWorldEditor : public GuiSystem::AbstractEditor
{
    Q_OBJECT
    Q_DISABLE_COPY(HelloWorldEditor)

public:
    explicit HelloWorldEditor(QWidget *parent = 0);

    void open(const QUrl &);
    QUrl url() const;

    QIcon icon() const;
    QString title() const;
    QString windowTitle() const;

protected:
    void resizeEvent(QResizeEvent *);

private:
    QLabel *m_label;
};

class HelloWorldEditorFactory : public GuiSystem::AbstractEditorFactory
{
public:
    explicit HelloWorldEditorFactory(QObject *parent = 0);

    QStringList mimeTypes();
    QByteArray id() const;

protected:
    GuiSystem::AbstractEditor *createEditor(QWidget *parent);
};

#endif // HELLOWORLDEDITOR_H
