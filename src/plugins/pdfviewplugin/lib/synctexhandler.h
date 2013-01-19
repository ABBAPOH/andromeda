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

#ifndef PDFVIEWER_SYNCTEXHANDLER_H
#define PDFVIEWER_SYNCTEXHANDLER_H

#include "synctex/synctex_parser.h"
#include <QtCore/QObject>
#include <QtCore/QRectF>

struct SynctexTextBox
{
	int pageNumber;
	QRectF textBox;
};

class SynctexHandler : public QObject
{
	Q_OBJECT

public:
	explicit SynctexHandler(QObject *parent = 0);
	~SynctexHandler();

	void loadData(const QString &fileName);
	void removeData();
	void synctexClick(const QPointF &pagePos, int pageNumber);
	QList<SynctexTextBox> syncFromSource(const QString &sourceFile, int lineNumber);

Q_SIGNALS:
	void openTexDocument(const QString &fileName, int lineNumber);

private:
	synctex_scanner_t m_synctexScanner;
	QString m_fileName;
};

#endif // PDFVIEWER_SYNCTEXHANDLER_H
