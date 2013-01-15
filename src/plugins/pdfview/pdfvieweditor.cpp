/*
 * Copyright (C) 2012, Glad Deschrijver <glad.deschrijver@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 */

#include "pdfviewdocument.h"
#include "pdfvieweditor.h"
#include "lib/pdfview.h"
#include "findwidget.h"
#include "utils/icon.h"

#include <GuiSystem/constants.h>

#include <QtCore/QSettings>
#include <QtCore/QUrl>
#include <QtGui/QActionGroup>
#include <QtGui/QApplication>
#include <QtGui/QFileDialog>
#include <QtGui/QLabel>
#include <QtGui/QMessageBox>
#include <QtGui/QResizeEvent>
#include <QtGui/QToolBar>
#include <QtGui/QVBoxLayout>

using namespace GuiSystem;
//using namespace PdfView;

PdfViewEditor::PdfViewEditor(QWidget *parent) :
    AbstractEditor(*new PdfViewDocument, parent),
    m_findWidget(0)
{
    document()->setParent(this);
    connect(document(), SIGNAL(urlChanged(QUrl)), SLOT(open(QUrl)));

    m_pdfView = new PdfView(this);
    m_pdfView->setZoomFactor(1);
    m_pdfView->setMaximumFileSettingsCacheSize(50e6);

    // load actions
    m_pdfView->action(PdfView::ZoomIn)->setIcon(Icon("zoom-in"));
    m_pdfView->action(PdfView::ZoomOut)->setIcon(Icon("zoom-out"));

    // Tools
    m_mouseBrowseAction = m_pdfView->action(PdfView::MouseToolBrowse);
    m_mouseBrowseAction->setIcon(Icon("input-mouse"));
    m_mouseBrowseAction->setShortcut(tr("Ctrl+1"));

    m_mouseMagnifyAction = m_pdfView->action(PdfView::MouseToolMagnify);
    m_mouseMagnifyAction->setIcon(Icon("page-zoom"));
    m_mouseMagnifyAction->setShortcut(tr("Ctrl+2"));

    m_mouseSelectionAction = m_pdfView->action(PdfView::MouseToolSelection);
    m_mouseSelectionAction->setIcon(Icon("select-rectangular"));
    m_mouseSelectionAction->setShortcut(tr("Ctrl+3"));

    m_mouseTextSelectionAction = m_pdfView->action(PdfView::MouseToolTextSelection);
    m_mouseTextSelectionAction->setIcon(Icon("draw-text"));
    m_mouseTextSelectionAction->setShortcut(tr("Ctrl+4"));

    m_pdfView->addContextMenuAction(m_mouseBrowseAction);
    m_pdfView->addContextMenuAction(m_mouseMagnifyAction);
    m_pdfView->addContextMenuAction(m_mouseSelectionAction);
    m_pdfView->addContextMenuAction(m_mouseTextSelectionAction);

    // save
    QAction *saveAsAction = new QAction(tr("Save &As...", "Action"), this);
    saveAsAction->setObjectName(Constants::Actions::SaveAs);
    connect(saveAsAction, SIGNAL(triggered()), this, SLOT(slotSaveCopy()));
    addAction(saveAsAction);

    // find
    QAction *findAction = new QAction(tr("&Find...", "Action"), this);
    findAction->setObjectName(Constants::Actions::Find);
    connect(findAction, SIGNAL(triggered()), this, SLOT(slotOpenFind()));
    addAction(findAction);

    QAction *findNextAction = new QAction(tr("Find &Next", "Action"), this);
    findNextAction->setObjectName(Constants::Actions::FindNext);
    connect(findNextAction, SIGNAL(triggered()), this, SLOT(slotFindNext()));
    addAction(findNextAction);

    QAction *findPreviousAction = new QAction(tr("Find &Previous", "Action"), this);
    findPreviousAction->setObjectName(Constants::Actions::FindPrevious);
    connect(findPreviousAction, SIGNAL(triggered()), this, SLOT(slotFindPrevious()));
    addAction(findPreviousAction);

    m_pdfView->action(PdfView::ZoomIn)->setObjectName(Constants::Actions::ZoomIn);
    m_pdfView->action(PdfView::ZoomOut)->setObjectName(Constants::Actions::ZoomOut);

    QToolBar *toolBar = new QToolBar(this);
    toolBar->addAction(m_pdfView->action(PdfView::ZoomIn));
    toolBar->addAction(m_pdfView->action(PdfView::ZoomOut));
    toolBar->addSeparator();
    toolBar->addAction(m_pdfView->action(PdfView::MouseToolBrowse));
    toolBar->addAction(m_pdfView->action(PdfView::MouseToolMagnify));
    toolBar->addAction(m_pdfView->action(PdfView::MouseToolSelection));
    toolBar->addAction(m_pdfView->action(PdfView::MouseToolTextSelection));
    toolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(toolBar);
    layout->addWidget(m_pdfView);

    readSettings();
}

PdfViewEditor::~PdfViewEditor()
{
    close();
    delete m_pdfView;
    m_pdfView = 0;
}

void PdfViewEditor::close()
{
    m_pdfView->close();
}

QUrl PdfViewEditor::url() const
{
//    const QString fileName = m_pdfView->fileName();
//    return QUrl::fromLocalFile(fileName);
    return m_url;
}

QIcon PdfViewEditor::icon() const
{
    return QIcon(":/icons/pdfview.png");
}

QString PdfViewEditor::title() const
{
    const QString fileName = m_pdfView->fileName();
    Poppler::Document *doc = m_pdfView->document();
    if (doc)
        return doc->info("Title");
    if (!fileName.isEmpty())
        return fileName;
    return tr("PDF View");
}

QString PdfViewEditor::windowTitle() const
{
    const QString fileName = m_pdfView->fileName();
    Poppler::Document *doc = m_pdfView->document();
    if (doc)
        return doc->info("Title");
    if (!fileName.isEmpty())
        return fileName;
    return tr("PDF View");
}

/*******************************************************************/
// Mouse tool

void PdfViewEditor::readSettings()
{
    QSettings settings(this);
    settings.beginGroup("pdfView");
    const int mouseToolNumber = settings.value("MouseTool", 1).toInt();
    switch (mouseToolNumber)
    {
        case 0: m_mouseBrowseAction->setChecked(true); break;
        case 1: m_mouseMagnifyAction->setChecked(true); break;
        case 2: m_mouseSelectionAction->setChecked(true); break;
        case 3: m_mouseTextSelectionAction->setChecked(true); break;
    }
    selectMouseTool(mouseToolNumber); // for some strange reason the above actions don't trigger their signal, so we must explicitly select the mouse tool of the view here
    settings.endGroup();
}

void PdfViewEditor::selectMouseTool(int which)
{
    switch (which)
    {
        case 0: m_pdfView->setMouseTool(PdfView::Browsing); break;
        case 1: m_pdfView->setMouseTool(PdfView::Magnifying); break;
        case 2: m_pdfView->setMouseTool(PdfView::Selection); break;
        case 3: m_pdfView->setMouseTool(PdfView::TextSelection); break;
    }
}

void PdfViewEditor::open(const QUrl &url)
{
    m_pdfView->load(url.toLocalFile());
}

void PdfViewEditor::slotSelectMouseTool()
{
    QAction *action = qobject_cast<QAction*>(sender());
    if (action)
    {
        int which = action->data().toInt();
        selectMouseTool(which);
        QSettings settings(this);
        settings.beginGroup("pdfView");
        settings.setValue("MouseTool", which);
        settings.endGroup();
    }
}

/*******************************************************************/
// Save

void PdfViewEditor::slotSaveCopy()
{
    if (!m_pdfView->document()) {
        return;
    }

    const QString fileName = QFileDialog::getSaveFileName(this, tr("Save Copy"), m_pdfView->fileName(), tr("PDF Documents (*.pdf)"));
    if (fileName.isEmpty()) {
        return;
    }

    Poppler::PDFConverter *converter = m_pdfView->document()->pdfConverter();
    converter->setOutputFileName(fileName);
    converter->setPDFOptions(converter->pdfOptions() & ~Poppler::PDFConverter::WithChanges);
    if (!converter->convert()) {
        QMessageBox msgbox(QMessageBox::Critical, tr("Save Error"), tr("Cannot export to:\n%1").arg(fileName),
                           QMessageBox::Ok, this);
    }
    delete converter;
}

/*******************************************************************/
// Find

void PdfViewEditor::slotSearch(const QString &text, const PdfView::FindFlags &flags)
{
    m_findText = text;
    m_findFlags = flags;
    QApplication::setOverrideCursor(Qt::WaitCursor);
    m_pdfView->search(text, flags);
    QApplication::restoreOverrideCursor();
}

void PdfViewEditor::slotOpenFind()
{
    if (!m_findWidget)
    {
        m_findWidget = new FindWidget(this);
        layout()->addWidget(m_findWidget);
        connect(m_findWidget, SIGNAL(search(QString,PdfView::FindFlags)), this, SLOT(slotSearch(QString,PdfView::FindFlags)));
        connect(m_findWidget, SIGNAL(focusEditor()), m_pdfView, SLOT(setFocus()));
        connect(m_pdfView, SIGNAL(closeFindWidget()), m_findWidget, SLOT(hide()));
    }
    m_findWidget->setVisible(true);
    m_findWidget->setFocus();
}

void PdfViewEditor::slotFindNext()
{
    if (m_findText.isEmpty())
    {
        slotOpenFind();
        m_findWidget->setForward(true);
    }
    else
        slotSearch(m_findText, m_findFlags & ~PdfView::FindBackward);
}

void PdfViewEditor::slotFindPrevious()
{
    if (m_findText.isEmpty())
    {
        slotOpenFind();
        m_findWidget->setForward(false);
    }
    else
    slotSearch(m_findText, m_findFlags | PdfView::FindBackward);
}

/*******************************************************************/
// Factory

PdfViewEditorFactory::PdfViewEditorFactory(QObject *parent) :
    AbstractEditorFactory(parent)
{
}

QByteArray PdfViewEditorFactory::id() const
{
    return "PdfView";
}

AbstractEditor * PdfViewEditorFactory::createEditor(QWidget *parent)
{
    return new PdfViewEditor(parent);
}
