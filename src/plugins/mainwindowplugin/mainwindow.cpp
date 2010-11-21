#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <editorview.h>
#include <QLineEdit>
#include <QToolButton>
#include <enteredlineedit.h>

using namespace CorePlugin;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setUnifiedTitleAndToolBarOnMac(true);

    m_lineEdit = new EnteredLineEdit(this);
    ui->toolBar->addWidget(m_lineEdit);

    m_newTabButton = new QToolButton(this);
    m_newTabButton->setText(tr("New Tab"));
    ui->tabWidget->setCornerWidget(m_newTabButton, Qt::TopRightCorner);

    connect(m_lineEdit, SIGNAL(textEntered(QString)), SLOT(open(QString)));
    connect(m_newTabButton, SIGNAL(clicked()), SLOT(newTab()));

    connect(ui->actionBack, SIGNAL(triggered()), SLOT(back()));
    connect(ui->actionForward, SIGNAL(triggered()), SLOT(forward()));

    newTab();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::newTab()
{
    ui->tabWidget->addTab(new EditorView, "tab");
}

void MainWindow::open(const QString &path)
{
    view()->open(path);
}

void MainWindow::back()
{
    view()->back();
}

void MainWindow::forward()
{
    view()->forward();
}

EditorView *MainWindow::view()
{
    return qobject_cast<EditorView *>(ui->tabWidget->currentWidget());
}
