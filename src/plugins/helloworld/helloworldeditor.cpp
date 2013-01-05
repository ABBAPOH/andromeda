#include "helloworldeditor.h"

#include <QtCore/QUrl>
#include <QtGui/QVBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QResizeEvent>

#include "helloworlddocument.h"

using namespace GuiSystem;
using namespace HelloWorld;

HelloWorldEditor::HelloWorldEditor(QWidget *parent) :
    AbstractEditor(*new HelloWorldDocument, parent),
    m_label(new QLabel(tr("Hello World!"), this))
{
    document()->setParent(this);
    m_label->setAlignment(Qt::AlignCenter);

    QLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(m_label);
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

AbstractDocument * HelloWorldEditorFactory::createDocument(QObject *parent)
{
    return new HelloWorldDocument(parent);
}

AbstractEditor * HelloWorldEditorFactory::createEditor(QWidget *parent)
{
    return new HelloWorldEditor(parent);
}
