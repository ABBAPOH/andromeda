//! [0]
class HELLOWORLD_EXPORT HelloWorldDocument : public GuiSystem::AbstractDocument
{
    Q_OBJECT
    Q_DISABLE_COPY(HelloWorldDocument)
public:
    explicit HelloWorldDocument(QObject *parent = 0);
    ~HelloWorldDocument();

public slots:
    void setTitle(const QString &title);

protected:
    bool openUrl(const QUrl &url);
};

HelloWorldDocument::HelloWorldDocument(QObject *parent) :
    AbstractDocument(parent)
{
    setIcon(QIcon(":/icons/helloWorld.png"));
    setTitle(tr("Hello world"));
}

HelloWorldDocument::~HelloWorldDocument()
{
}

void HelloWorldDocument::setTitle(const QString &title)
{
    AbstractDocument::setTitle(title);
}

bool HelloWorldDocument::openUrl(const QUrl &/*url*/)
{
    return true;
}
//! [0]

//! [1]
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
//! [1]

//! [2]
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
//! [2]

//! [3]
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
//! [3]

//! [4]
void HelloWorldEditor::init()
{
    m_label->setAlignment(Qt::AlignCenter);

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
//! [4]

//! [5]
void HelloWorldEditor::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu;
    menu.addAction(tr("Change text..."), this, SLOT(changeText()));
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
//! [5]

//! [6]
class HelloWorldEditorFactory : public GuiSystem::AbstractEditorFactory
{
public:
    explicit HelloWorldEditorFactory(QObject *parent = 0);

    QByteArray id() const;
    QString name() const;
    QIcon icon() const;

protected:
    GuiSystem::AbstractDocument *createDocument(QObject *parent);
    GuiSystem::AbstractEditor *createEditor(QWidget *parent);
};

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
//! [6]

//! [7]
class HelloWorldPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_DISABLE_COPY(HelloWorldPlugin)
public:
    HelloWorldPlugin();
    ~HelloWorldPlugin();

    bool initialize();
};

HelloWorldPlugin::HelloWorldPlugin() :
    ExtensionSystem::IPlugin()
{
}

HelloWorldPlugin::~HelloWorldPlugin()
{
}

bool HelloWorldPlugin::initialize()
{
    HelloWorldEditorFactory *f = new HelloWorldEditorFactory(this);
    EditorManager::instance()->addFactory(f);

    return true;
}
//! [7]


