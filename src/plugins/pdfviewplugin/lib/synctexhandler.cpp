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
 *
 * The functions loadData(), removeData(), synctexClick() and
 * syncFromSource() are inspired from similar functions in TeXworks which is
 * Copyright (C) 2007-2011 Jonathan Kew, Stefan Löffler
 * licensed under the GPL version 2 or later,
 * see <http://www.tug.org/texworks/>.
 */

#include "synctexhandler.h"

#include <QtCore/QDir>
#include <QtCore/QFileInfo>

SynctexHandler::SynctexHandler(QObject *parent)
	: QObject(parent)
	, m_synctexScanner(0)
{
}

SynctexHandler::~SynctexHandler()
{
}

void SynctexHandler::loadData(const QString &fileName)
{
	removeData();

	m_fileName = fileName;
	m_synctexScanner = synctex_scanner_new_with_output_file(m_fileName.toUtf8().data(), 0, 1);
}

void SynctexHandler::removeData()
{
	if (m_synctexScanner)
	{
		synctex_scanner_free(m_synctexScanner);
		m_synctexScanner = 0;
	}
}

void SynctexHandler::synctexClick(const QPointF &pagePos, int pageNumber)
{
	if (!m_synctexScanner)
		return;

	if (synctex_edit_query(m_synctexScanner, pageNumber + 1, pagePos.x(), pagePos.y()) > 0)
	{
		synctex_node_t synctexNode;
		while ((synctexNode = synctex_next_result(m_synctexScanner)) != 0)
		{
			const QString texFileName = QString::fromUtf8(synctex_scanner_get_name(m_synctexScanner, synctex_node_tag(synctexNode)));
			const QDir currentDir(QFileInfo(m_fileName).canonicalPath());
			Q_EMIT openTexDocument(QFileInfo(currentDir, texFileName).canonicalFilePath(), synctex_node_line(synctexNode));
		}
	}
}

QList<SynctexTextBox> SynctexHandler::syncFromSource(const QString &sourceFile, int lineNumber)
{
	QList<SynctexTextBox> textBoxList;

	if (!m_synctexScanner)
		return textBoxList;

	// find the name synctex is using for this file
	const QFileInfo sourceFileInfo(sourceFile);
	const QDir currentDir(QFileInfo(m_fileName).canonicalPath());
	synctex_node_t synctexNode = synctex_scanner_input(m_synctexScanner);
	QString texFileName;
	bool found = false;
	while (synctexNode)
	{
		texFileName = QString::fromUtf8(synctex_scanner_get_name(m_synctexScanner, synctex_node_tag(synctexNode)));
		if (QFileInfo(currentDir, texFileName) == sourceFileInfo)
		{
			found = true;
			break;
		}
		synctexNode = synctex_node_sibling(synctexNode);
	}
	if (!found)
		return textBoxList;

	if (synctex_display_query(m_synctexScanner, texFileName.toUtf8().data(), lineNumber, 0) > 0)
	{
		int pageNumber = -1;
		while ((synctexNode = synctex_next_result(m_synctexScanner)) != 0)
		{
			if (pageNumber < 0)
				pageNumber = synctex_node_page(synctexNode);
			if (synctex_node_page(synctexNode) != pageNumber)
				continue;
			const QRectF textBox(synctex_node_box_visible_h(synctexNode),
			    synctex_node_box_visible_v(synctexNode) - synctex_node_box_visible_height(synctexNode),
			    synctex_node_box_visible_width(synctexNode),
			    synctex_node_box_visible_height(synctexNode));
			SynctexTextBox synctexTextBox;
			synctexTextBox.pageNumber = pageNumber;
			synctexTextBox.textBox = textBox;
			textBoxList << synctexTextBox;
		}
	}
	return textBoxList;
}
