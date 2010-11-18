#include "mainwindow.h"

#include <editorview.h>

#include <QtGui/QToolBar>
#include <QtGui/QLineEdit>
#include <QtGui/QToolButton>

using namespace MainWindowPlugin;
using namespace Core;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    m_toolBar = new QToolBar(this);
    m_lineEdit = new QLineEdit(this);
    m_toolBar->addWidget(m_lineEdit);
    addToolBar(m_toolBar);
    setUnifiedTitleAndToolBarOnMac(true);

    m_newTabButton = new QToolButton(this);
    m_newTabButton->setText(tr("New Tab"));
//    m_newTabButton->setFlat(true);
    m_tabWidget = new QTabWidget(this);
    m_tabWidget->setCornerWidget(m_newTabButton, Qt::TopRightCorner);
    m_tabWidget->setDocumentMode(true);
    setCentralWidget(m_tabWidget);
    newTab();
    resize(640, 480);

    connect(m_lineEdit, SIGNAL(textEdited(QString)), SLOT(open(QString)));
    connect(m_newTabButton, SIGNAL(clicked()), SLOT(newTab()));
}

void MainWindow::newTab()
{
    m_tabWidget->addTab(new EditorView, "tab");
}

void MainWindow::open(const QString &path)
{
    view()->open(path);
}

EditorView *MainWindow::view()
{
    return qobject_cast<EditorView *>(m_tabWidget->currentWidget());
}
