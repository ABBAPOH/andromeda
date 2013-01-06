#include "helloworlddocument.h"

using namespace GuiSystem;
using namespace HelloWorld;

/*!
    \class HelloWorld::HelloWorldDocument

    HelloWorldDocument serves as a model for a HelloWorldEditor - it holds
    editor's text in an AbstractDocument::title property.
*/

/*!
    Creates HelloWorldEditor with the given \a parent.
*/
HelloWorldDocument::HelloWorldDocument(QObject *parent) :
    AbstractDocument(parent)
{
    setIcon(QIcon(":/icons/helloWorld.png"));
    setTitle(tr("Hello world"));
}

/*!
    Destroys HelloWorldEditor.
*/
HelloWorldDocument::~HelloWorldDocument()
{
}

/*!
    Setter for an AbstractDocument::title property.
*/
void HelloWorldDocument::setTitle(const QString &title)
{
    AbstractDocument::setTitle(title);
}

/*!
    \reimp
*/
bool HelloWorldDocument::openUrl(const QUrl &url)
{
    return true;
}
