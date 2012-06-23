/***************************************************************************
 *   Copyright (C) 2008, 2011, 2012 by Glad Deschrijver                    *
 *     <glad.deschrijver@gmail.com>                                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/

#ifndef PDFVIEWER_FINDWIDGET_H
#define PDFVIEWER_FINDWIDGET_H

#include "ui_findwidget.h"
#include "pdfview.h"

class FindWidget : public QWidget
{
	Q_OBJECT

public:
	FindWidget(QWidget *parent = 0);
	~FindWidget();
	void setForward(bool forward);
	void setText(const QString &text);

Q_SIGNALS:
	void focusEditor();
	void search(const QString &text, PdfView::FindFlags flags);

protected Q_SLOTS:
	void hide();

protected:
	virtual void showEvent(QShowEvent *event);
	virtual void keyPressEvent(QKeyEvent *event);
	Ui::FindWidget ui;

private Q_SLOTS:
	void setBackward();
	void setForward();
	void doFind();
};

#endif // PDFVIEWER_FINDWIDGET_H
