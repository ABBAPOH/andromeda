#ifndef HELLOWORLDEDITOR_H
#define HELLOWORLDEDITOR_H

#include "helloworld_global.h"

#include <GuiSystem/AbstractEditor>
#include <GuiSystem/AbstractEditorFactory>

class QLabel;

namespace HelloWorld {

class HelloWorldDocument;
class HELLOWORLD_EXPORT HelloWorldEditor : public GuiSystem::AbstractEditor
{
    Q_OBJECT
    Q_DISABLE_COPY(HelloWorldEditor)
public:
    explicit HelloWorldEditor(QWidget *parent = 0);
    explicit HelloWorldEditor(HelloWorldDocument &document, QWidget *parent = 0);
    ~HelloWorldEditor();

    void setDocument(GuiSystem::AbstractDocument *document);

protected:
    void contextMenuEvent(QContextMenuEvent *event);

private slots:
    void changeText();

private:
    void init();
    void setupDocument(HelloWorldDocument *document);

private:
    QLabel *m_label;
};

class HelloWorldEditorFactory : public GuiSystem::AbstractEditorFactory
{
    Q_OBJECT
    Q_DISABLE_COPY(HelloWorldEditorFactory)
public:
    explicit HelloWorldEditorFactory(QObject *parent = 0);

    QByteArray id() const;
    QString name() const;
    QIcon icon() const;

protected:
    GuiSystem::AbstractDocument *createDocument(QObject *parent);
    GuiSystem::AbstractEditor *createEditor(QWidget *parent);
};

} // namespace HelloWorld

#endif // HELLOWORLDEDITOR_H
