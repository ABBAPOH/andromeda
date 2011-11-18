#include "helloworldeditor.h"

#include <QtCore/QUrl>
#include <QtGui/QLabel>
#include <QtGui/QResizeEvent>

using namespace GuiSystem;

HelloWorldEditor::HelloWorldEditor(QWidget *parent) :
    AbstractEditor(parent),
    m_label(new QLabel(tr("Hello World!"), this))
{
    m_label->setAlignment(Qt::AlignCenter);
}

void HelloWorldEditor::open(const QUrl &url)
{
    emit urlChanged(url);
}

QUrl HelloWorldEditor::url() const
{
    return QUrl(QLatin1String("andromeda://helloworld"));
}

QIcon HelloWorldEditor::icon() const
{
    return QIcon(":/icons/helloWorld.png");
}

QString HelloWorldEditor::title() const
{
    return tr("Hello world");
}

QString HelloWorldEditor::windowTitle() const
{
    return tr("Hello world");
}

void HelloWorldEditor::resizeEvent(QResizeEvent *e)
{
    m_label->resize(e->size());
}

HelloWorldEditorFactory::HelloWorldEditorFactory(QObject *parent) :
    AbstractEditorFactory(parent)
{
}

QStringList HelloWorldEditorFactory::mimeTypes()
{
    return QStringList();
}

QByteArray HelloWorldEditorFactory::id() const
{
    return "helloworld";
}

AbstractEditor * HelloWorldEditorFactory::createEditor(QWidget *parent)
{
    return new HelloWorldEditor(parent);
}
