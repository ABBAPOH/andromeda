#include "helloworldeditor.h"

#include <QtCore/QUrl>
#include <QtGui/QVBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QResizeEvent>

using namespace GuiSystem;
using namespace HelloWorld;

HelloWorldEditor::HelloWorldEditor(QWidget *parent) :
    AbstractEditor(parent),
    m_label(new QLabel(tr("Hello World!"), this))
{
    m_label->setAlignment(Qt::AlignCenter);

    QLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(m_label);
}

void HelloWorldEditor::open(const QUrl &)
{
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

HelloWorldEditorFactory::HelloWorldEditorFactory(QObject *parent) :
    AbstractEditorFactory(parent)
{
}

QByteArray HelloWorldEditorFactory::id() const
{
    return "helloworld";
}

QString HelloWorldEditorFactory::name() const
{
    return tr("Hello world editor");
}

QIcon HelloWorldEditorFactory::icon() const
{
    return QIcon(":/icons/helloWorld.png");
}

AbstractEditor * HelloWorldEditorFactory::createEditor(QWidget *parent)
{
    return new HelloWorldEditor(parent);
}
