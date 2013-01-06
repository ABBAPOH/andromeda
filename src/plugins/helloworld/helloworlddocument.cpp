#include "helloworlddocument.h"

using namespace GuiSystem;
using namespace HelloWorld;

HelloWorldDocument::HelloWorldDocument(QObject *parent) :
    AbstractDocument(parent)
{
    setIcon(QIcon(":/icons/helloWorld.png"));
    setTitle(tr("Hello world"));
}

void HelloWorldDocument::setTitle(const QString &title)
{
    AbstractDocument::setTitle(title);
}
