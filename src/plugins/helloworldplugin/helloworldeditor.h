#ifndef HELLOWORLDEDITOR_H
#define HELLOWORLDEDITOR_H

#include <coreplugin/abstracteditor.h>
#include <coreplugin/abstracteditorfactory.h>

class QLabel;
class HelloWorldEditor : public CorePlugin::AbstractEditor
{
    Q_OBJECT
    Q_DISABLE_COPY(HelloWorldEditor)

public:
    explicit HelloWorldEditor(QWidget *parent = 0);

    bool open(const QUrl &url);
    QUrl currentUrl() const;

    QIcon icon() const;
    QString title() const;
    QString windowTitle() const;

protected:
    void resizeEvent(QResizeEvent *);

private:
    QLabel *m_label;
};

class HelloWorldEditorFactory : public CorePlugin::AbstractEditorFactory
{
public:
    explicit HelloWorldEditorFactory(QObject *parent = 0);

    QStringList mimeTypes();
    QByteArray id() const;

protected:
    CorePlugin::AbstractEditor *createEditor(QWidget *parent);
};

#endif // HELLOWORLDEDITOR_H
