/***************************************************************************
 *   Copyright (C) 2012 by Glad Deschrijver                                *
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

#ifndef PDFVIEWER_PAGEITEM_H
#define PDFVIEWER_PAGEITEM_H

#include <QtCore/QObject>
#include <poppler-qt4.h>

struct Link
{
	QRectF linkArea;
	double pageNumber;
	QString pageLabel;
	QString url;
	Poppler::LinkAction::ActionType actionType;
};
//Q_DECLARE_TYPEINFO(Link, Q_PRIMITIVE_TYPE);

class PageItem : public QObject
{
	Q_OBJECT

public:
	PageItem(QObject *parent = 0);

	void generateLinks(Poppler::Page *popplerPage, const QStringList &popplerPageLabels);
	QList<Link> links() const;
	static QString toolTipText(const Link &link);

private:
	QList<Link> m_links;
};

#endif // PDFVIEWER_PAGEITEM_H
