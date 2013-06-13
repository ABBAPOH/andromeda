#include "browsertabwidget.h"

#include <QtCore/QDataStream>
#include <QtCore/QDir>

#include <Parts/AbstractDocument>
#include <Parts/EditorView>
#include <Widgets/TabBarButton>

using namespace Andromeda;
using namespace Parts;

/*!
    \class Andromeda::BrowserTabWidget
    \internal
*/

/*!
    Constructs BrowserTabWidget with the given \a parent.
*/
BrowserTabWidget::BrowserTabWidget(QWidget *parent) :
    TabWidget(parent),
    m_newTabButton(new TabBarButton)
{
    m_newTabButton->setIcon(QIcon(":/andromeda/icons/addtab.png"));
    m_newTabButton->setIconSize(QSize(32, 32));

    setDocumentMode(true);
    setMovable(true);
    setUsesScrollButtons(true);
    setCornerWidget(m_newTabButton, Qt::TopRightCorner);
    setTabBarVisible(false);

    m_defaultUrl = QUrl::fromLocalFile(QDir::homePath());

    connect(this, SIGNAL(currentChanged(int)), SLOT(onCurrentChanged(int)));
    connect(this, SIGNAL(tabBarDoubleClicked()), SLOT(newTab()));
    connect(this, SIGNAL(tabCloseRequested(int)), SLOT(closeTab(int)));
    connect(m_newTabButton, SIGNAL(clicked()), SLOT(newTab()));
}

/*!
    Destroys BrowserTabWidget.
*/
BrowserTabWidget::~BrowserTabWidget()
{
}

/*!
    \brief Url to be opened in new tab.
*/
QUrl BrowserTabWidget::defaultUrl() const
{
    return m_defaultUrl;
}

void BrowserTabWidget::setDefaultUrl(const QUrl &url)
{
    m_defaultUrl = url;
}

AbstractDocument * BrowserTabWidget::currentDocument() const
{
    AbstractEditor *editor = currentEditor();
    if (!editor)
        return 0;
    return editor->document();
}

AbstractEditor * BrowserTabWidget::currentEditor() const
{
    return currentView()->editor();
}

EditorView * BrowserTabWidget::currentView() const
{
    return qobject_cast<EditorView *>(currentWidget());
}

/*!
    Restores tab widget's state.
*/
bool BrowserTabWidget::restoreState(const QByteArray &arr)
{
    QByteArray state = arr;
    QDataStream s(&state, QIODevice::ReadOnly);

    int currentIndex = 0;
    int tabCount = 0;
    s >> currentIndex;
    s >> tabCount;

    for (int i = 0; i < tabCount; i++) {
        QByteArray childState;
        s >> childState;
        EditorView *view = createView();
        addView(view, false);
        bool ok = view->restoreState(childState);
        if (!ok)
            return false;
    }

    setCurrentIndex(currentIndex);

    return true;
}

/*!
    Saves Browser tab widget state.
*/
QByteArray BrowserTabWidget::saveState() const
{
    QByteArray state;
    QDataStream s(&state, QIODevice::WriteOnly);

    int index = currentIndex();
    int tabCount = count();

    s << index;
    s << tabCount;
    for (int i = 0; i < tabCount; i++) {
        EditorView *view = qobject_cast<EditorView *>(widget(i));
        s << view->saveState();
    }

    return state;
}

/*!
    \brief Opens stacked editor in tab new.
*/
void BrowserTabWidget::newTab()
{
    newTab(m_defaultUrl);
}

void BrowserTabWidget::newTab(const QUrl &url)
{
    EditorView *view = createView();
    addView(view);
    view->open(url);
}

void BrowserTabWidget::open(const QUrl &url)
{
    if (count() == 0) {
        newTab(url);
    } else {
        currentView()->open(url);
    }
}

void BrowserTabWidget::openEditor(const QList<QUrl> &urls, const QByteArray &editorId)
{
    // open single url in a current tab
    if (urls.count() == 1) {
        EditorView *view = currentView();
        view->openEditor(urls.first(), editorId);
        return;
    }

    foreach (const QUrl &url, urls) {
        EditorView *view = createView();
        addView(view, false);
        view->openEditor(url, editorId);
    }
}

void BrowserTabWidget::closeTab(int index)
{
    if (count() <= 1) {
        return;
    }

    if (index == -1) {
        index = currentIndex();
    }

    if (index == currentIndex())
        // switch to other tab before closing to prevent losing focus
        setCurrentIndex(index ? index - 1 : count() - 1);

    QWidget *widget = this->widget(index);
    removeTab(index);
    widget->deleteLater();

    if (count() == 1) {
        setTabsClosable(false);
        setTabBarVisible(false);
    }
}

void BrowserTabWidget::close()
{
    closeTab(-1);
}

void BrowserTabWidget::addView(EditorView *view, bool changeTab)
{
    int index = addTab(view, tr("New tab"));
    if (changeTab) {
        if (index != currentIndex())
            setCurrentIndex(index);
    }

    bool closable = count() > 1;
    setTabsClosable(closable);
    setTabBarVisible(closable);
}

void BrowserTabWidget::onEditorChanged()
{
    EditorView *view = qobject_cast<EditorView *>(sender());
    Q_ASSERT(view);

    AbstractDocument *document = view->document();
    if (!document)
        return;

    int index = indexOf(document);
    if (index == -1)
        return;

#ifndef Q_OS_MAC
    setTabIcon(index, document->icon());
#endif
    setTabText(index, document->title());

    connect(document, SIGNAL(iconChanged(QIcon)), SLOT(onIconChanged(QIcon)));
    connect(document, SIGNAL(titleChanged(QString)), SLOT(onTitleChanged(QString)));

    emit editorChanged();
}

/*!
  \internal

  Reconnects old and current editors.
*/
void BrowserTabWidget::onCurrentChanged(int /*index*/)
{
//    setView(currentView());
    emit editorChanged();
}

/*!
  \internal
*/
void BrowserTabWidget::onIconChanged(const QIcon &icon)
{
#ifdef Q_OS_MAC
    Q_UNUSED(icon)
#else
    AbstractDocument *doc = qobject_cast<AbstractDocument *>(sender());

    int index = indexOf(doc);
    setTabIcon(index, icon);
#endif
}

/*!
    \internal
*/
void BrowserTabWidget::onTitleChanged(const QString &title)
{
    AbstractDocument *doc = qobject_cast<AbstractDocument *>(sender());

    int index = indexOf(doc);
    setTabText(index, title);
}

/*!
    \internal
*/
EditorView * BrowserTabWidget::createView()
{
    EditorView *view = new EditorView(this);

    connect(view, SIGNAL(editorChanged()), SLOT(onEditorChanged()));

    return view;
}

/*!
    \internal

    Returns the index position of the page occupied by the \a document, or -1
    if the document cannot be found.
*/
int BrowserTabWidget::indexOf(AbstractDocument *document)
{
    for (int i = 0; i < this->count(); ++i) {
        EditorView *container = qobject_cast<EditorView *>(this->widget(i));
        if (container->editor()->document() == document)
            return i;
    }

    return -1;
}

void BrowserTabWidget::setView(EditorView *view)
{
    if (m_view == view)
        return;

    if (m_view)
        disconnect(m_view, SIGNAL(editorChanged()), this, SLOT(onEditorChanged()));

    m_view = view;

    if (m_view)
        connect(m_view, SIGNAL(editorChanged()), this, SLOT(onEditorChanged()));

    emit viewChanged();
}

void BrowserTabWidget::setDocument(AbstractDocument *document)
{
    if (m_document == document)
        return;

    if (m_document) {
        disconnect(m_document, SIGNAL(iconChanged(QIcon)), this, SLOT(onIconChanged(QIcon)));
        disconnect(m_document, SIGNAL(titleChanged(QString)), this, SLOT(onTitleChanged(QString)));
    }

    m_document = document;

    if (m_document) {
        connect(m_document, SIGNAL(iconChanged(QIcon)), this, SLOT(onIconChanged(QIcon)));
        connect(m_document, SIGNAL(titleChanged(QString)), this, SLOT(onTitleChanged(QString)));
    }
}

int BrowserTabWidget::indexOf(QWidget *widget)
{
    return TabWidget::indexOf(widget);
}
