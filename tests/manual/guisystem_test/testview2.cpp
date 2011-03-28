#include "testview2.h"

#include <QtWebKit>
#include <QtGui>

TestView2::TestView2(QObject *parent) :
    IView()
{
    QWebView *widget = new QWebView();
    widget->setMinimumSize(200, 200);
    widget->setUrl(QUrl("http://qt.nokia.com"));
    m_widget = widget;
    m_toolBar = new QToolBar;
    m_toolBar->addAction("Third action");
    m_toolBar->addAction("Fourth action");
}
