#ifndef HELLOWORLDEDITOR_H
#define HELLOWORLDEDITOR_H

#include <Parts/AbstractEditor>
#include <Parts/AbstractEditorFactory>

class QLabel;

namespace HelloWorld {

class HelloWorldDocument;
class HelloWorldEditor : public Parts::AbstractEditor
{
    Q_OBJECT
    Q_DISABLE_COPY(HelloWorldEditor)
public:
    explicit HelloWorldEditor(QWidget *parent = 0);
    explicit HelloWorldEditor(HelloWorldDocument &document, QWidget *parent = 0);
    ~HelloWorldEditor();

    void setDocument(Parts::AbstractDocument *document);

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

class HelloWorldEditorFactory : public Parts::AbstractEditorFactory
{
    Q_OBJECT
    Q_DISABLE_COPY(HelloWorldEditorFactory)
public:
    explicit HelloWorldEditorFactory(QObject *parent = 0);

protected:
    Parts::AbstractEditor *createEditor(QWidget *parent);
};

} // namespace HelloWorld

#endif // HELLOWORLDEDITOR_H
