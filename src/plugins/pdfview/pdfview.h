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

#ifndef PDFVIEWER_PDFVIEW_H
#define PDFVIEWER_PDFVIEW_H

#include <QtGui/QGraphicsView>
#include <poppler-qt4.h>

class PdfViewPrivate;
//class ZoomAction;
//namespace Poppler {
//	class Document;
//	class Page;
//}

class PdfView : public QGraphicsView
{
    Q_OBJECT

	Q_PROPERTY(QString fileName READ fileName)
//	Q_PROPERTY(Poppler::Document document READ document)
	Q_PROPERTY(qreal zoomFactor READ zoomFactor WRITE setZoomFactor NOTIFY zoomFactorChanged)
	Q_PROPERTY(double pageNumber READ pageNumberWithPosition WRITE setPage)
	Q_PROPERTY(QStringList pageLabels READ popplerPageLabels)
	Q_ENUMS(MouseTool PdfViewAction)

public:
	enum MouseTool {
		Browsing = 0,
		Magnifying = 1,
		Selection = 2,
		TextSelection = 3
	};
	enum FindFlag {
		FindBackward = 0x00001,
		FindCaseSensitively = 0x00002
	};
	Q_DECLARE_FLAGS(FindFlags, FindFlag)
	enum PositionHandling {
		DontKeepPosition = 0,
		DontUpdatePosition = 1,
		KeepPosition = 2,
		ReloadPage = 3
	};
	enum PdfViewAction {
		NoPdfViewAction = -1,
		ZoomIn,
		ZoomOut,
		Zoom,
		GoToStartOfDocument,
		GoToEndOfDocument,
		GoToPreviousPage,
		GoToNextPage,
		GoToPage,
		Bookmarks,
		SetBookmark,
		PreviousBookmark,
		NextBookmark
	};

	explicit PdfView(QWidget *parent = 0);
	~PdfView();

	void setMouseTool(MouseTool mouseTool);
	void setMaximumFileSettingsCacheSize(int size);

	QAction *action(PdfViewAction action);

	/**
	 * This function maps a point in page coordinates to scene coordinates.
	 * \param pageNumber the number of the page to which the coordinates refer
	 * \param point the point in page coordinates
	 * \return the point in scene coordinates
	 */
	QPointF mapFromPage(int pageNumber, const QPointF &point) const;
	/**
	 * This function maps a rectangle in page coordinates to scene coordinates.
	 * \param pageNumber the number of the page to which the coordinates refer
	 * \param rect the rectangle in page coordinates
	 * \return the rectangle in scene coordinates
	 */
	QRectF mapFromPage(int pageNumber, const QRectF &rect) const;
	/**
	 * This function maps a point in scene coordinates to page coordinates.
	 * \param pageNumber the number of the page to which the coordinates refer
	 * \param point the point in scene coordinates
	 * \return the point in page coordinates
	 */
	QPointF mapToPage(int pageNumber, const QPointF &point) const;
	/**
	 * This function maps a rectangle in scene coordinates to page coordinates.
	 * \param pageNumber the number of the page to which the coordinates refer
	 * \param rect the rectangle in scene coordinates
	 * \return the rectangle in page coordinates
	 */
	QRectF mapToPage(int pageNumber, const QRectF &rect) const;

	void setRenderBackend(Poppler::Document::RenderBackend backend);
	Poppler::Document::RenderBackend renderBackend() const;
	void setRenderHint(Poppler::Document::RenderHint hint, bool on = true);
	Poppler::Document::RenderHints renderHints() const;

	bool load(const QString &fileName);
	QString fileName() const;
	Poppler::Document *document() const;
//	void loadFileSpecificSettings();
	void close();
	QList<Poppler::Page*> popplerPages();
	QStringList popplerPageLabels();

	/*virtual*/ void setPage(double pageNumber, PositionHandling keepPosition = DontKeepPosition);
	double pageNumberWithPosition() const;

	void search(const QString &text, const FindFlags &flags);
	void setZoomFactor(qreal value);
	qreal zoomFactor() const;

	void syncFromSource(const QString &sourceFile, int lineNumber);

	void addContextMenuAction(QAction *action);
	void removeContextMenuAction(QAction *action);

public Q_SLOTS:
	void slotZoomIn();
	void slotZoomOut();
	void slotGoToStartOfDocument();
	void slotGoToEndOfDocument();
	void slotGoToPreviousPage();
	void slotGoToNextPage();
	void slotGoToPage();

Q_SIGNALS:
	void scrollPositionChanged(qreal fraction, int pageNumber);
	void openTexDocument(const QString &fileName, int lineNumber);
	void closeFindWidget();
	void zoomFactorChanged(qreal value);

protected:
#ifndef QT_NO_CONTEXTMENU
	/*virtual*/ void contextMenuEvent(QContextMenuEvent *event);
#endif // QT_NO_CONTEXTMENU
	/*virtual*/ void keyPressEvent(QKeyEvent *event);
	/*virtual*/ void mousePressEvent(QMouseEvent *event);
	/*virtual*/ void mouseMoveEvent(QMouseEvent *event);
	/*virtual*/ void mouseReleaseEvent(QMouseEvent *event);
#ifndef QT_NO_WHEELEVENT
	/*virtual*/ void wheelEvent(QWheelEvent *event);
#endif // QT_NO_WHEELEVENT

private:
//	QSharedDataPointer<PdfViewPrivate> d;
	PdfViewPrivate *d;
	friend class PdfViewPrivate;
};

#endif // PDFVIEWER_PDFVIEW_H
