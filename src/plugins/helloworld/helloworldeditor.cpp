#include "helloworldeditor.h"

#include <QtCore/QUrl>
#include <QtGui/QVBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QMenu>
#include <QtGui/QResizeEvent>
#include <QtGui/QInputDialog>

#include "helloworlddocument.h"

using namespace GuiSystem;
using namespace HelloWorld;

HelloWorldEditor::HelloWorldEditor(QWidget *parent) :
    AbstractEditor(*new HelloWorldDocument, parent),
    m_label(new QLabel(this))
{
    document()->setParent(this);
    init();
}

HelloWorldEditor::HelloWorldEditor(HelloWorldDocument &document, QWidget *parent) :
    AbstractEditor(document, parent),
    m_label(new QLabel(this))
{
    init();
}

void HelloWorldEditor::setDocument(AbstractDocument *document)
{
    HelloWorldDocument *helloDocument = qobject_cast<HelloWorldDocument *>(document);
    if (!helloDocument)
        return;

    if (this->document())
        disconnect(this->document(), SIGNAL(titleChanged(QString)), m_label, SLOT(setText(QString)));

    setupDocument(helloDocument);

    AbstractEditor::setDocument(document);
}

void HelloWorldEditor::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu;
    menu.addAction(tr("Change text"), this, SLOT(changeText()));
    menu.exec(event->globalPos());
}

void HelloWorldEditor::changeText()
{
    HelloWorldDocument *helloDocument = qobject_cast<HelloWorldDocument *>(document());

    QInputDialog dialog(this);
    dialog.setInputMode(QInputDialog::TextInput);
    dialog.setWindowTitle(tr("Change text"));
    dialog.setLabelText(tr("Type new text:"));
    dialog.setTextValue(helloDocument->title());
    if (!dialog.exec())
        return;

    helloDocument->setTitle(dialog.textValue());
}

void HelloWorldEditor::init()
{
    m_label->setAlignment(Qt::AlignCenter);
    m_label->setContextMenuPolicy(Qt::NoContextMenu);

    QLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(m_label);

    setContextMenuPolicy(Qt::DefaultContextMenu);

    setupDocument(qobject_cast<HelloWorldDocument *>(document()));
}

void HelloWorldEditor::setupDocument(HelloWorldDocument *document)
{
    Q_ASSERT(document);

    m_label->setText(document->title());

    connect(document, SIGNAL(titleChanged(QString)), m_label, SLOT(setText(QString)));
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
