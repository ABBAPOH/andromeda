#include "bineditor.h"

#include <QtGui/QAction>
#include <QtGui/QResizeEvent>
#include <QtGui/QFileIconProvider>

#include <guisystem/constants.h>

#include "binedit.h"
#include "bineditordocument.h"

using namespace GuiSystem;
using namespace BINEditor;

BinEditor::BinEditor(QWidget *parent) :
    AbstractEditor(*new BinEditorDocument, parent),
    m_editor(new BinEdit(this))
{
    document()->setParent(this);
    createActions();
    retranslateUi();
    registerActions();

    connect(document(), SIGNAL(urlChanged(QUrl)), this, SLOT(open(QUrl)));
}

void BinEditor::setDocument(AbstractDocument *document)
{
    if (this->document() == document)
        return;

    BinEditorDocument *binDocument = qobject_cast<BinEditorDocument *>(document);
    if (!binDocument)
        return;

    connect(document, SIGNAL(urlChanged(QUrl)), this, SLOT(open(QUrl)));

    AbstractEditor::setDocument(document);
}

void BinEditor::open(const QUrl &url)
{
    m_editor->open(url.toLocalFile());
}

void BinEditor::resizeEvent(QResizeEvent *e)
{
    m_editor->resize(e->size());
}

void BinEditor::createActions()
{
    actions[BinEditor::Redo] = new QAction(this);
    actions[BinEditor::Redo]->setEnabled(false);
    connect(m_editor, SIGNAL(redoAvailable(bool)), actions[BinEditor::Redo], SLOT(setEnabled(bool)));
    connect(actions[BinEditor::Redo], SIGNAL(triggered()), m_editor, SLOT(redo()));

    actions[BinEditor::Undo] = new QAction(this);
    actions[BinEditor::Undo]->setEnabled(false);
    connect(m_editor, SIGNAL(undoAvailable(bool)), actions[BinEditor::Undo], SLOT(setEnabled(bool)));
    connect(actions[BinEditor::Undo], SIGNAL(triggered()), m_editor, SLOT(undo()));

    actions[BinEditor::Copy] = new QAction(this);
    actions[BinEditor::Copy]->setEnabled(false);
    connect(m_editor, SIGNAL(copyAvailable(bool)), actions[BinEditor::Copy], SLOT(setEnabled(bool)));
    connect(actions[BinEditor::Copy], SIGNAL(triggered()), m_editor, SLOT(copy()));

    actions[BinEditor::SelectAll] = new QAction(this);
    connect(actions[BinEditor::SelectAll], SIGNAL(triggered()), m_editor, SLOT(selectAll()));
}

void BinEditor::retranslateUi()
{
    actions[BinEditor::Redo]->setText(tr("Redo"));
    actions[BinEditor::Undo]->setText(tr("Undo"));
    actions[BinEditor::Copy]->setText(tr("Copy"));
    actions[BinEditor::SelectAll]->setText(tr("Select all"));
}

void BinEditor::registerActions()
{
    addAction(actions[BinEditor::Redo], Constants::Actions::Redo);
    addAction(actions[BinEditor::Undo], Constants::Actions::Undo);
    addAction(actions[BinEditor::Copy], Constants::Actions::Copy);
    addAction(actions[BinEditor::SelectAll], Constants::Actions::SelectAll);
}

BinEditorFactory::BinEditorFactory(QObject *parent) :
    AbstractEditorFactory(parent)
{
}

QStringList BinEditorFactory::mimeTypes() const
{
    return QStringList() << "application/octet-stream";
}

int BinEditorFactory::weight() const
{
    return 20;
}

QByteArray BinEditorFactory::id() const
{
    return "bineditor";
}

QString BinEditorFactory::name() const
{
    return tr("Binary editor");
}

QIcon BinEditorFactory::icon() const
{
    return QIcon();
}

AbstractDocument * BinEditorFactory::createDocument(QObject *parent)
{
    return new BinEditorDocument(parent);
}

AbstractEditor * BinEditorFactory::createEditor(QWidget *parent)
{
    return new BinEditor(parent);
}
