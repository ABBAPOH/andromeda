#include "plaintextedit.h"

#include <QtGui/QAction>
#include <QtGui/QFont>

using namespace TextEditor;

PlainTextEdit::PlainTextEdit(QWidget *parent) :
    QPlainTextEdit(parent)
{
    setWordWrapMode(QTextOption::NoWrap);

    createActions();
}

void PlainTextEdit::createActions()
{
    actions[PlainTextEdit::New] = new QAction(this);
    actions[PlainTextEdit::New]->setObjectName("actionNew");
    actions[PlainTextEdit::New]->setShortcut(QKeySequence::New);
    actions[PlainTextEdit::New]->setText(tr("New"));
    actions[PlainTextEdit::New]->setIcon(QIcon::fromTheme("document-new"));
    connect(actions[PlainTextEdit::New], SIGNAL(triggered()), this, SLOT(clear()));

    actions[PlainTextEdit::Save] = new QAction(this);
    actions[PlainTextEdit::Save]->setObjectName("Save");
    actions[PlainTextEdit::Save]->setShortcut(QKeySequence::Save);
    actions[PlainTextEdit::Save]->setText(tr("Save"));
    actions[PlainTextEdit::Save]->setIcon(QIcon::fromTheme("document-save"));

    actions[PlainTextEdit::SaveAs] = new QAction(this);
    actions[PlainTextEdit::SaveAs]->setObjectName("SaveAs");
    actions[PlainTextEdit::SaveAs]->setShortcut(QKeySequence::SaveAs);
    actions[PlainTextEdit::SaveAs]->setText(tr("Save As"));
    actions[PlainTextEdit::SaveAs]->setIcon(QIcon::fromTheme("document-save-as"));

    actions[PlainTextEdit::Undo] = new QAction(this);
    actions[PlainTextEdit::Undo]->setObjectName("Undo");
    actions[PlainTextEdit::Undo]->setShortcut(QKeySequence::Undo);
    actions[PlainTextEdit::Undo]->setText(tr("Undo"));
    actions[PlainTextEdit::Undo]->setIcon(QIcon::fromTheme("edit-undo"));
    connect(actions[PlainTextEdit::Undo], SIGNAL(triggered()), this, SLOT(undo()));
    connect(this, SIGNAL(undoAvailable(bool)), actions[PlainTextEdit::Undo], SLOT(setEnabled(bool)));
    actions[PlainTextEdit::Undo]->setEnabled(false);

    actions[PlainTextEdit::Redo] = new QAction(this);
    actions[PlainTextEdit::Redo]->setObjectName("Redo");
    actions[PlainTextEdit::Redo]->setShortcut(QKeySequence::Redo);
    actions[PlainTextEdit::Redo]->setText(tr("Redo"));
    actions[PlainTextEdit::Redo]->setIcon(QIcon::fromTheme("edit-redo"));
    connect(actions[PlainTextEdit::Redo], SIGNAL(triggered()), this, SLOT(redo()));
    connect(this, SIGNAL(redoAvailable(bool)), actions[PlainTextEdit::Redo], SLOT(setEnabled(bool)));
    actions[PlainTextEdit::Redo]->setEnabled(false);

    actions[PlainTextEdit::Copy] = new QAction(this);
    actions[PlainTextEdit::Copy]->setObjectName("Copy");
    actions[PlainTextEdit::Copy]->setShortcut(QKeySequence::Copy);
    actions[PlainTextEdit::Copy]->setText(tr("Copy"));
    actions[PlainTextEdit::Copy]->setIcon(QIcon::fromTheme("edit-copy"));
    connect(actions[PlainTextEdit::Copy], SIGNAL(triggered()), this, SLOT(copy()));
    connect(this, SIGNAL(copyAvailable(bool)), actions[PlainTextEdit::Copy], SLOT(setEnabled(bool)));
    actions[PlainTextEdit::Copy]->setEnabled(false);

    actions[PlainTextEdit::Cut] = new QAction(this);
    actions[PlainTextEdit::Cut]->setObjectName("Cut");
    actions[PlainTextEdit::Cut]->setShortcut(QKeySequence::Cut);
    actions[PlainTextEdit::Cut]->setText(tr("Cut"));
    actions[PlainTextEdit::Cut]->setIcon(QIcon::fromTheme("edit-cut"));
    connect(actions[PlainTextEdit::Cut], SIGNAL(triggered()), this, SLOT(cut()));
    connect(this, SIGNAL(copyAvailable(bool)), actions[PlainTextEdit::Cut], SLOT(setEnabled(bool)));
    actions[PlainTextEdit::Cut]->setEnabled(false);

    actions[PlainTextEdit::Paste] = new QAction(this);
    actions[PlainTextEdit::Paste]->setObjectName("Paste");
    actions[PlainTextEdit::Paste]->setShortcut(QKeySequence::Paste);
    actions[PlainTextEdit::Paste]->setText(tr("Paste"));
    actions[PlainTextEdit::Paste]->setIcon(QIcon::fromTheme("edit-paste"));
    connect(actions[PlainTextEdit::Paste], SIGNAL(triggered()), this, SLOT(paste()));

    actions[PlainTextEdit::ZoomIn] = new QAction(this);
    actions[PlainTextEdit::ZoomIn]->setObjectName("ZoomIn");
    actions[PlainTextEdit::ZoomIn]->setShortcut(QKeySequence::ZoomIn);
    actions[PlainTextEdit::ZoomIn]->setText(tr("Zoom In"));
    actions[PlainTextEdit::ZoomIn]->setIcon(QIcon::fromTheme("zoom-in"));
    connect(actions[PlainTextEdit::ZoomIn], SIGNAL(triggered()), this, SLOT(zoomIn()));

    actions[PlainTextEdit::ZoomOut] = new QAction(this);
    actions[PlainTextEdit::ZoomOut]->setObjectName("ZoomOut");
    actions[PlainTextEdit::ZoomOut]->setShortcut(QKeySequence::ZoomOut);
    actions[PlainTextEdit::ZoomOut]->setText(tr("Zoom Out"));
    actions[PlainTextEdit::ZoomOut]->setIcon(QIcon::fromTheme("zoom-out"));
    connect(actions[PlainTextEdit::ZoomOut], SIGNAL(triggered()), this, SLOT(zoomOut()));
}

void PlainTextEdit::zoomIn()
{
    QFont font(this->font());
    font.setPointSize(font.pointSize() + 3);
    this->setFont(font);
}

void PlainTextEdit::zoomOut()
{
    QFont font(this->font());
    font.setPointSize(font.pointSize() - 3);
    this->setFont(font);
}

QAction * PlainTextEdit::action(PlainTextEdit::Action action) const
{
    if (action < 0 || action >= ActionsCount)
        return 0;

    return actions[action];
}
