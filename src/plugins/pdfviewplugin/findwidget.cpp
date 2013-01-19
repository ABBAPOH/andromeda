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

#include "findwidget.h"

#include <QtGui/QKeyEvent>
#include "utils/icon.h"
#include "utils/lineedit.h"

FindWidget::FindWidget(QWidget *parent) : QWidget(parent)
{
	ui.setupUi(this);
	ui.comboBoxFind->setLineEdit(new LineEdit(this));
	ui.pushButtonClose->setIcon(Icon("dialog-cancel"));
	ui.pushButtonBackward->setIcon(Icon("go-up"));
	ui.pushButtonForward->setIcon(Icon("go-down"));

	setFocusProxy(ui.comboBoxFind);

	connect(ui.pushButtonBackward, SIGNAL(clicked()), this, SLOT(setBackward()));
	connect(ui.pushButtonForward, SIGNAL(clicked()), this, SLOT(setForward()));
	connect(ui.pushButtonFind, SIGNAL(clicked()), this, SLOT(doFind()));
	connect(ui.pushButtonClose, SIGNAL(clicked()), this, SLOT(hide()));
}

FindWidget::~FindWidget()
{
}

void FindWidget::setBackward()
{
	ui.pushButtonBackward->setChecked(true);
	ui.pushButtonForward->setChecked(false);
}

void FindWidget::setForward()
{
	ui.pushButtonBackward->setChecked(false);
	ui.pushButtonForward->setChecked(true);
}

void FindWidget::setForward(bool forward)
{
	if (forward)
		setForward();
	else
		setBackward();
}

void FindWidget::hide()
{
	setVisible(false);
	emit focusEditor();
}

void FindWidget::doFind()
{
	const QString currentText = ui.comboBoxFind->currentText();
	if (currentText.isEmpty())
		return;
	if (ui.comboBoxFind->findText(currentText) < 0)
		ui.comboBoxFind->addItem(currentText);

	PdfView::FindFlags flags = 0;
	if (ui.checkBoxCaseSensitive->isChecked())
		flags |= PdfView::FindCaseSensitively;
	if (!ui.pushButtonForward->isChecked())
		flags |= PdfView::FindBackward;
	emit search(currentText, flags);
}

void FindWidget::setText(const QString &text)
{
	ui.comboBoxFind->lineEdit()->setText(text);
	ui.comboBoxFind->setFocus();
	ui.comboBoxFind->lineEdit()->selectAll();
}

void FindWidget::showEvent(QShowEvent *event)
{
	ui.comboBoxFind->setFocus();
	ui.comboBoxFind->lineEdit()->selectAll();
	QWidget::showEvent(event);
}

void FindWidget::keyPressEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_Escape)
		hide();
	else if (event->key() == Qt::Key_Return)
		doFind();
	QWidget::keyPressEvent(event);
}
