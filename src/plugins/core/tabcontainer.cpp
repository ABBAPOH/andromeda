#include "tabcontainer.h"

#include "tabwidget.h"

#include <QtCore/QDataStream>
#include <QtGui/QDesktopServices>
#include <QtGui/QResizeEvent>
#include <QtGui/QTabWidget>

#include <guisystem/stackedcontainer.h>

using namespace Core;
using namespace GuiSystem;

/*!
  \class TabContainer

  \brief This class implements AbstractContainer interface for a TabWidget.
*/

/*!
  \brief Constructs TabContainer with the given \a parent.
*/
TabContainer::TabContainer(QWidget *parent) :
    AbstractContainer(parent),
    m_tabWidget(new MyTabWidget(this)),
    m_newTabButton(new TabBarButton),
    m_editor(0),
    m_proxyHistory(new ProxyHistory(this))
{
    m_newTabButton->setIcon(QIcon(":/images/icons/addtab.png"));
    m_newTabButton->setIconSize(QSize(32, 32));

    m_tabWidget->setDocumentMode(true);
    m_tabWidget->setMovable(true);
    m_tabWidget->setUsesScrollButtons(true);
    m_tabWidget->setCornerWidget(m_newTabButton, Qt::TopRightCorner);

    m_defaultUrl = QUrl::fromLocalFile(QDesktopServices::storageLocation(QDesktopServices::HomeLocation));

    connect(m_tabWidget, SIGNAL(currentChanged(int)), SLOT(onCurrentChanged(int)));
    connect(m_tabWidget, SIGNAL(tabBarDoubleClicked()), SLOT(newTab()));
    connect(m_tabWidget, SIGNAL(tabCloseRequested(int)), SLOT(closeEditor(int)));
    connect(m_newTabButton, SIGNAL(clicked()), SLOT(newTab()));
}

/*!
  \reimp
*/
int TabContainer::count() const
{
    return m_tabWidget->count();
}

/*!
  \reimp
*/
int TabContainer::currentIndex() const
{
    return m_tabWidget->currentIndex();
}

/*!
  \reimp
*/
void TabContainer::setCurrentIndex(int index)
{
    m_tabWidget->setCurrentIndex(index);
}

/*!
  \brief Opens stacked editor in tab new.
*/
void TabContainer::newTab()
{
    openNewEditor(m_defaultUrl);
}

/*!
  \reimp
*/
AbstractEditor * TabContainer::editor(int index) const
{
    return qobject_cast<AbstractEditor *>(m_tabWidget->widget(index));
}

/*!
  \brief Url to be opened in new tab.
*/
QUrl TabContainer::defaultUrl() const
{
    return m_defaultUrl;
}

void TabContainer::setDefaultUrl(const QUrl &url)
{
    m_defaultUrl = url;
}

/*!
  \reimp
*/
AbstractHistory * TabContainer::history() const
{
    return m_proxyHistory;
}

/*!
  \brief Returns inner tab widget.
*/
MyTabWidget * TabContainer::tabWidget() const
{
    return m_tabWidget;
}

/*!
  \reimp
*/
bool TabContainer::restoreState(const QByteArray &arr)
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
        AbstractEditor *container = createEditor();
        bool ok = container->restoreState(childState);
        if (!ok)
            return false;
        QString title = container->title();
#ifdef Q_OS_MAC
        m_tabWidget->addTab(container, title.isEmpty() ? tr("New tab") : title);
#else
        m_tabWidget->addTab(container, container->icon(), title.isEmpty() ? tr("New tab") : title);
#endif
    }

    m_tabWidget->setCurrentIndex(currentIndex);
    m_tabWidget->setTabsClosable(tabCount <= 1);
    StackedContainer *container = qobject_cast<StackedContainer *>(m_tabWidget->currentWidget());
    setEditor(container);

    return true;
}

/*!
  \reimp
*/
QByteArray TabContainer::saveState() const
{
    QByteArray state;
    QDataStream s(&state, QIODevice::WriteOnly);

    int currentIndex = m_tabWidget->currentIndex();
    int tabCount = m_tabWidget->count();

    s << currentIndex;
    s << tabCount;
    for (int i = 0; i < tabCount; i++) {
        AbstractEditor *e = editor(i);
        s << e->saveState();
    }

    return state;
}

/*!
  \reimp
*/
void TabContainer::openNewEditor(const QUrl &url)
{
    AbstractEditor *container = createEditor();
    container->open(url);
    QString title = container->title();

    int index;
#ifdef Q_OS_MAC
    index = m_tabWidget->addTab(container, title.isEmpty() ? tr("New tab") : title);
#else
    index = m_tabWidget->addTab(container, container->icon(), title.isEmpty() ? tr("New tab") : title);
#endif

    if (index != m_tabWidget->currentIndex())
        m_tabWidget->setCurrentIndex(index);
    else
        setEditor(container);

    if (m_tabWidget->count() > 1)
        m_tabWidget->setTabsClosable(true);
}

/*!
  \reimp
*/
void TabContainer::closeEditor(int index)
{
    if (m_tabWidget->count() <= 1) {
        return;
    }

    if (index == -1) {
        index = m_tabWidget->currentIndex();
    }

    if (index == m_tabWidget->currentIndex())
        m_tabWidget->setCurrentIndex(index ? index - 1 : m_tabWidget->count() - 1); // switch to other tab before closing to prevent losing focus

    QWidget *widget = m_tabWidget->widget(index);
    m_tabWidget->removeTab(index);
    widget->deleteLater();

    if (m_tabWidget->count() == 1)
        m_tabWidget->setTabsClosable(false);
}

/*!
  \reimp
*/
void Core::TabContainer::resizeEvent(QResizeEvent *e)
{
    m_tabWidget->resize(e->size());
}

/*!
  \internal

  Reconnects old and current editors.
*/
void TabContainer::onCurrentChanged(int index)
{
    if (m_editor) {
        disconnectEditor(m_editor);
        emit loadFinished(true);
    }

    setEditor(editor(index));

    emit currentChanged(index);
}

/*!
  \internal
*/
void TabContainer::onIconChanged(const QIcon &icon)
{
    AbstractEditor *editor = qobject_cast<AbstractEditor *>(sender());

    int index = m_tabWidget->indexOf(editor);
#ifndef Q_OS_MAC
    m_tabWidget->setTabIcon(index, icon);
#endif

    if (index == m_tabWidget->currentIndex())
        emit iconChanged(icon);
}

/*!
  \internal
*/
void TabContainer::onTitleChanged(const QString &title)
{
    AbstractEditor *editor = qobject_cast<AbstractEditor *>(sender());

    int index = m_tabWidget->indexOf(editor);
    m_tabWidget->setTabText(index, title);

    if (index == m_tabWidget->currentIndex())
        emit titleChanged(title);
}

/*!
  \internal
*/
AbstractEditor * TabContainer::createEditor()
{
    StackedContainer *editor = new StackedContainer(m_tabWidget);

    connect(editor, SIGNAL(iconChanged(QIcon)), this, SLOT(onIconChanged(QIcon)));
    connect(editor, SIGNAL(titleChanged(QString)), this, SLOT(onTitleChanged(QString)));

    return editor;
}

/*!
  \internal
*/
void TabContainer::setEditor(AbstractEditor *editor)
{
    m_editor = editor;
    m_proxyHistory->setSourceHistory(m_editor->history());
    connectEditor(m_editor);

    emit urlChanged(m_editor->url());
    emit iconChanged(m_editor->icon());
    emit titleChanged(m_editor->title());
    emit windowTitleChanged(m_editor->windowTitle());
}

/*!
  \internal
*/
void TabContainer::connectEditor(AbstractEditor *editor)
{
    connect(editor, SIGNAL(urlChanged(QUrl)),
            this, SIGNAL(urlChanged(QUrl)));
    connect(editor, SIGNAL(openTriggered(QUrl)), this, SLOT(open(QUrl)));
    connect(editor, SIGNAL(openNewEditorTriggered(QList<QUrl>)),
            this, SIGNAL(openNewEditorTriggered(QList<QUrl>)));
    connect(editor, SIGNAL(openNewWindowTriggered(QList<QUrl>)),
            this, SIGNAL(openNewWindowTriggered(QList<QUrl>)));

    connect(editor, SIGNAL(windowTitleChanged(QString)), this, SIGNAL(windowTitleChanged(QString)));

    connect(editor, SIGNAL(loadStarted()), this, SIGNAL(loadStarted()));
    connect(editor, SIGNAL(loadProgress(int)), this, SIGNAL(loadProgress(int)));
    connect(editor, SIGNAL(loadFinished(bool)), this, SIGNAL(loadFinished(bool)));

    connect(editor, SIGNAL(capabilitiesChanged(Capabilities)),
            this, SIGNAL(capabilitiesChanged(Capabilities)));
}

/*!
  \internal
*/
void TabContainer::disconnectEditor(AbstractEditor *editor)
{
    disconnect(m_editor, 0, this, 0);

    // these connections should be always present to update tab text and icon
    connect(editor, SIGNAL(iconChanged(QIcon)), this, SLOT(onIconChanged(QIcon)));
    connect(editor, SIGNAL(titleChanged(QString)), this, SLOT(onTitleChanged(QString)));
}
