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

#ifndef PDFVIEWEDITOR_H
#define PDFVIEWEDITOR_H

#include "pdfview_global.h"
#include "lib/pdfview.h"

#include <guisystem/abstracteditor.h>
#include <guisystem/abstracteditorfactory.h>

//class PdfView;
class FindWidget;

//namespace PdfView {

class PdfViewDocument;

class PDFVIEW_EXPORT PdfViewEditor : public GuiSystem::AbstractEditor
{
    Q_OBJECT
    Q_DISABLE_COPY(PdfViewEditor)

public:
    explicit PdfViewEditor(QWidget *parent = 0);
    ~PdfViewEditor();

    GuiSystem::AbstractDocument *document() const;

    void close();
    QUrl url() const;

    QIcon icon() const;
    QString title() const;
    QString windowTitle() const;

private Q_SLOTS:
    void open(const QUrl &url);
    void slotSelectMouseTool();
    void slotSaveCopy();
    void slotSearch(const QString &text, const PdfView::FindFlags &flags);
    void slotOpenFind();
    void slotFindNext();
    void slotFindPrevious();

private:
    void readSettings();
    void selectMouseTool(int which);

    QUrl m_url;

    PdfView *m_pdfView;
    QAction *m_mouseBrowseAction;
    QAction *m_mouseMagnifyAction;
    QAction *m_mouseSelectionAction;
    QAction *m_mouseTextSelectionAction;
    FindWidget *m_findWidget;
    QString m_findText;
    PdfView::FindFlags m_findFlags;
};

class PdfViewEditorFactory : public GuiSystem::AbstractEditorFactory
{
public:
    explicit PdfViewEditorFactory(QObject *parent = 0);

    QByteArray id() const;
    QString name() const;
    QIcon icon() const;
    QStringList mimeTypes() const;
    int weight() const;

protected:
    GuiSystem::AbstractDocument *createDocument(QObject *parent);
    GuiSystem::AbstractEditor *createEditor(QWidget *parent);
};

//} // namespace PdfView

#endif // PDFVIEWEDITOR_H
