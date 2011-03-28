#include "testview.h"

#include <QPlainTextEdit>
#include <QToolBar>

TestView::TestView(QObject *parent) :
    IView()
{
    m_widget = new QPlainTextEdit();
    m_toolBar = new QToolBar;
    m_toolBar->addAction("First action");
    m_toolBar->addAction("Second action");
}

