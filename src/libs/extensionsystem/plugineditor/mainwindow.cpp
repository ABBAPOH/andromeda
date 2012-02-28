#include "mainwindow.h"

#include <QtCore/QSettings>
#include <QtGui/QFileDialog>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>

using namespace ExtensionSystem;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_editor(new PluginEditor)
{
    settings = new QSettings(this);
    setupUi();
    setCentralWidget(m_editor);
    resize(450, 600);
    pluginSpec = new MutablePluginSpec(this);
    m_editor->setPluginSpec(pluginSpec);
}

MainWindow::~MainWindow()
{
    settings->setValue("format", format());
}

void MainWindow::open()
{
    QString dir = settings->value("fileDialogDir").toString();
    QString file = QFileDialog::getOpenFileName(this, tr("Open"), dir, QLatin1String("*.spec"));

    if (file.isEmpty())
        return;

    dir = QFileInfo(file).absolutePath();
    settings->setValue("fileDialogDir", dir);

    open(file);
}

void MainWindow::open(const QString &file)
{
    currentFile = file;
    if (!pluginSpec->read(file)) {
        return;
    }
    m_editor->reset();
}

void MainWindow::save()
{
    if (currentFile.isEmpty())
        return;

    save(currentFile);
}

void MainWindow::save(const QString &file)
{
    m_editor->apply();
    if (!pluginSpec->write(file, format())) {
        return;
    }
}

void MainWindow::saveAs()
{
    QString dir = settings->value("fileDialogDir").toString();
    QString file = QFileDialog::getSaveFileName(this, tr("Save"), dir, QLatin1String("*.spec"));

    if (file.isEmpty())
        return;

    dir = QFileInfo(file).absolutePath();
    settings->setValue("fileDialogDir", dir);

    save(file);
}

void MainWindow::setupUi()
{
    setWindowTitle(tr("PluginEditor"));

    fileMenu = new QMenu(tr("File"));

    openAction = fileMenu->addAction(tr("Open..."), this, SLOT(open()), QKeySequence("Ctrl+O"));
    saveAction = fileMenu->addAction(tr("Save"), this, SLOT(save()), QKeySequence("Ctrl+S"));
    saveAsAction = fileMenu->addAction(tr("Save As..."), this, SLOT(saveAs()), QKeySequence("Ctrl+Shift+S"));
    fileMenu->addSeparator();
    quitAction = fileMenu->addAction(tr("Quit PluginEditor"), this, SLOT(close()), QKeySequence("Ctrl+Q"));

    menuBar()->addMenu(fileMenu);

    formatMenu = new QMenu(tr("Format"));
    QActionGroup *formatGroup = new QActionGroup(formatMenu);
    formatGroup->setExclusive(true);

    int format = settings->value("format").toInt();

    saveXmlAction = formatMenu->addAction(tr("Xml format"));
    saveXmlAction->setCheckable(true);
    if (format == 0)
        saveXmlAction->setChecked(true);
    formatGroup->addAction(saveXmlAction);

    saveBinaryAction = formatMenu->addAction(tr("Binary format"));
    saveBinaryAction->setCheckable(true);
    if (format == 1)
        saveBinaryAction->setChecked(true);
    formatGroup->addAction(saveBinaryAction);

    menuBar()->addMenu(formatMenu);
}

PluginSpec::Format MainWindow::format()
{
    if (saveXmlAction->isChecked())
        return PluginSpec::XmlFormat;
    if (saveBinaryAction->isChecked())
        return PluginSpec::BinaryFormat;
    return PluginSpec::BinaryFormat;
}
