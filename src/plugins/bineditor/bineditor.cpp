#include "bineditor.h"

#include <QtGui/QAction>
#include <QtGui/QResizeEvent>
#include <QtGui/QFileIconProvider>

#include <GuiSystem/constants.h>

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
    actions[BinEditor::Redo]->setObjectName(Constants::Actions::Redo);
    addAction(actions[BinEditor::Redo]);
    connect(m_editor, SIGNAL(redoAvailable(bool)), actions[BinEditor::Redo], SLOT(setEnabled(bool)));
    connect(actions[BinEditor::Redo], SIGNAL(triggered()), m_editor, SLOT(redo()));

    actions[BinEditor::Undo] = new QAction(this);
    actions[BinEditor::Undo]->setEnabled(false);
    actions[BinEditor::Undo]->setObjectName(Constants::Actions::Undo);
    addAction(actions[BinEditor::Undo]);
    connect(m_editor, SIGNAL(undoAvailable(bool)), actions[BinEditor::Undo], SLOT(setEnabled(bool)));
    connect(actions[BinEditor::Undo], SIGNAL(triggered()), m_editor, SLOT(undo()));

    actions[BinEditor::Copy] = new QAction(this);
    actions[BinEditor::Copy]->setEnabled(false);
    actions[BinEditor::Copy]->setObjectName(Constants::Actions::Copy);
    addAction(actions[BinEditor::Copy]);
    connect(m_editor, SIGNAL(copyAvailable(bool)), actions[BinEditor::Copy], SLOT(setEnabled(bool)));
    connect(actions[BinEditor::Copy], SIGNAL(triggered()), m_editor, SLOT(copy()));

    actions[BinEditor::SelectAll] = new QAction(this);
    actions[BinEditor::SelectAll]->setObjectName(Constants::Actions::SelectAll);
    addAction(actions[BinEditor::SelectAll]);
    connect(actions[BinEditor::SelectAll], SIGNAL(triggered()), m_editor, SLOT(selectAll()));
}

void BinEditor::retranslateUi()
{
    actions[BinEditor::Redo]->setText(tr("Redo"));
    actions[BinEditor::Undo]->setText(tr("Undo"));
    actions[BinEditor::Copy]->setText(tr("Copy"));
    actions[BinEditor::SelectAll]->setText(tr("Select all"));
}

/*!
    \class BinEditorFactory
*/

BinEditorFactory::BinEditorFactory(QObject *parent) :
    AbstractEditorFactory(parent)
{
}

/*!
    \reimp
*/
QByteArray BinEditorFactory::id() const
{
    return "bineditor";
}

/*!
    \reimp
*/
AbstractEditor * BinEditorFactory::createEditor(QWidget *parent)
{
    return new BinEditor(parent);
}
