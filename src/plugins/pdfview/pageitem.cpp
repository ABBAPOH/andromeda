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

#include <QDebug>
#include "pageitem.h"

#include <QtGui/QApplication>
#include <poppler-qt4.h>

PageItem::PageItem(QObject *parent)
    : QObject(parent)
{
}

/*******************************************************************/

/*
static QList<Link> getAnnotations(Poppler::Page *popplerPage)
{
    QList<Annotation> annotations;

    QList <Poppler::Annotation*> popplerAnnotations = popplerPage->annotations();
    annotations.reserve(popplerAnnotations.size());

    while (!popplerAnnotations.isEmpty())
    {
        Poppler::Annotation* popplerAnnotation = popplerAnnotations.takeFirst();
        Annotation annotation;
        switch (popplerAnnotation->subType())
        {
//			case Poppler::Annotation::AFileAttachment:
//			{
//				PopplerFileAttachmentAnnotation *attachAnnotation = static_cast<Poppler::FileAttachmentAnnotation*>(popplerAnnotation);
//				annotation.fileIconName = attachAnnotation->fileIconName();
//				annotation.embeddedFile = attachAnnotation->embeddedFile();
//			}
        }
        delete popplerAnnotation;
    }

    return annotations;
}

void Label::generateAnnotations(Poppler::Page *popplerPage)
{
    m_annotations.clear();
    m_annotations = getAnnotations(popplerPage);
}
*/

/*******************************************************************/

static QList<Link> getLinks(Poppler::Page *popplerPage, const QStringList &popplerPageLabels)
{
    QList<Link> links;

//QTime t = QTime::currentTime();
    QList <Poppler::Link*> popplerLinks = popplerPage->links(); // this is slow
//qCritical() << t.msecsTo(QTime::currentTime());
    links.reserve(popplerLinks.size());
    while (!popplerLinks.isEmpty())
    {
        Poppler::Link *popplerLink = popplerLinks.takeFirst();
        Link link;
//		link.linkArea = popplerLink->linkArea();
        const QRectF linkArea = popplerLink->linkArea();
        link.linkArea = QRectF(linkArea.left(), qMin(linkArea.top(), linkArea.bottom()), qAbs(linkArea.right() - linkArea.left()), qAbs(linkArea.bottom() - linkArea.top())); // poppler switches top and bottom of this box :(
        switch (popplerLink->linkType())
        {
            case Poppler::Link::Goto:
            {
                const Poppler::LinkGoto *popplerLinkGoto = static_cast<const Poppler::LinkGoto*>(popplerLink);
                const Poppler::LinkDestination popplerDest = popplerLinkGoto->destination();
                link.pageNumber = popplerDest.pageNumber() - 1 + popplerDest.top();
                link.pageLabel = popplerPageLabels.at(int(link.pageNumber));
            }
            break;
            case Poppler::Link::Browse:
            {
                const Poppler::LinkBrowse *popplerLinkBrowse = static_cast<const Poppler::LinkBrowse*>(popplerLink);
                link.url = popplerLinkBrowse->url();
            }
            break;
            case Poppler::Link::Action:
            {
                const Poppler::LinkAction *popplerLinkAction = static_cast<const Poppler::LinkAction*>(popplerLink);
                link.pageNumber = -1; // since Poppler::LinkAction::ActionType doesn't specify a "None" value, we use pageNumber to distinguish this type of action, we do not check whether popplerLinkAction->actionType() is > 0 because it is not documented that all valid action types have a value > 0
                link.actionType = popplerLinkAction->actionType();
            }
            break;
            case Poppler::Link::Execute: // TODO
            case Poppler::Link::Sound: // TODO
            case Poppler::Link::Movie: // not implemented in poppler 0.16
            case Poppler::Link::JavaScript: // TODO
            default: // do nothing
            break;
        }
        links << link;
        delete popplerLink;
    }

    return links;
}

void PageItem::generateLinks(Poppler::Page *popplerPage, const QStringList &popplerPageLabels)
{
    m_links.clear();
    m_links = getLinks(popplerPage, popplerPageLabels);
}

QList<Link> PageItem::links() const
{
    return m_links;
}

QString PageItem::toolTipText(const Link &link)
{
    QString toolTipText;

    if (!link.url.isEmpty()) // we have a Browse link
        return QApplication::translate("PageItem", "Go to %1", "Link action text").arg(link.url);
    else if (link.pageNumber >= 0) // we have a Goto link
        return QApplication::translate("PageItem", "Go to page %1", "Link action text").arg(link.pageLabel);

    switch (link.actionType)
    {
        case Poppler::LinkAction::PageFirst:
            toolTipText = QApplication::translate("PageItem", "Go to first page", "Link action text");
            break;
        case Poppler::LinkAction::PagePrev:
            toolTipText = QApplication::translate("PageItem", "Go to previous page", "Link action text");
            break;
        case Poppler::LinkAction::PageNext:
            toolTipText = QApplication::translate("PageItem", "Go to next page", "Link action text");
            break;
        case Poppler::LinkAction::PageLast:
            toolTipText = QApplication::translate("PageItem", "Go to last page", "Link action text");
            break;
        case Poppler::LinkAction::HistoryBack:
            toolTipText = QApplication::translate("PageItem", "Backward", "Link action text");
            break;
        case Poppler::LinkAction::HistoryForward:
            toolTipText = QApplication::translate("PageItem", "Forward", "Link action text");
            break;
        case Poppler::LinkAction::Quit:
            toolTipText = QApplication::translate("PageItem", "Quit application", "Link action text");
            break;
        case Poppler::LinkAction::Presentation:
            toolTipText = QApplication::translate("PageItem", "Enter presentation mode", "Link action text");
            break;
        case Poppler::LinkAction::EndPresentation:
            toolTipText = QApplication::translate("PageItem", "Exit presentation mode", "Link action text");
            break;
        case Poppler::LinkAction::Find:
            toolTipText = QApplication::translate("PageItem", "Find...", "Link action text");
            break;
        case Poppler::LinkAction::GoToPage:
            toolTipText = QApplication::translate("PageItem", "Go to page...", "Link action text");
            break;
        case Poppler::LinkAction::Close:
            toolTipText = QApplication::translate("PageItem", "Close document", "Link action text");
            break;
//		case Poppler::LinkAction::Print:
//			toolTipText = QApplication::translate("PageItem", "Print...", "Link action text");
//			break;
    }
    return toolTipText;
}
