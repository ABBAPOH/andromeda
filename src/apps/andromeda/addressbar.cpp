#include "addressbar.h"

#include <QtGui/QAction>
#include <guisystem/actionmanager.h>

#include "constants.h"

using namespace Andromeda;
using namespace GuiSystem;

MyAddressBar::MyAddressBar(QWidget *parent) :
    AddressBar(parent)
{
    createActions();
    retranslateUi();
    registerActions();
}

void MyAddressBar::createActions()
{
    actions[Redo] = new QAction(this);
    connect(actions[Redo], SIGNAL(triggered()), SLOT(redo()));

    actions[Undo] = new QAction(this);
    connect(actions[Undo], SIGNAL(triggered()), SLOT(undo()));

    actions[Cut] = new QAction(this);
    connect(actions[Cut], SIGNAL(triggered()), SLOT(cut()));

    actions[Copy] = new QAction(this);
    connect(actions[Copy], SIGNAL(triggered()), SLOT(copy()));

    actions[Paste] = new QAction(this);
    connect(actions[Paste], SIGNAL(triggered()), SLOT(paste()));

    actions[SelectAll] = new QAction(this);
    connect(actions[SelectAll], SIGNAL(triggered()), SLOT(selectAll()));

    for (int i = 0; i < ActionCount; i++) {
        addAction(actions[i]);
    }
}

void MyAddressBar::retranslateUi()
{
    actions[Redo]->setText(tr("Redo"));
    actions[Undo]->setText(tr("Undo"));

    actions[Cut]->setText(tr("Cut"));
    actions[Copy]->setText(tr("Copy"));
    actions[Paste]->setText(tr("Paste"));
    actions[SelectAll]->setText(tr("Select all"));
}

void MyAddressBar::registerActions()
{
    ActionManager *manager = ActionManager::instance();

    manager->registerAction(actions[Redo], Constants::Actions::Redo);
    manager->registerAction(actions[Undo], Constants::Actions::Undo);

    manager->registerAction(actions[Cut], Constants::Actions::Cut);
    manager->registerAction(actions[Copy], Constants::Actions::Copy);
    manager->registerAction(actions[Paste], Constants::Actions::Paste);
    manager->registerAction(actions[SelectAll], Constants::Actions::SelectAll);
}
