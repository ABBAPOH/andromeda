#ifndef TABCONTAINER_H
#define TABCONTAINER_H

#include <guisystem/proxyeditor.h>
#include <guisystem/proxyfile.h>
#include <guisystem/proxyhistory.h>

#include <QtCore/QPointer>

class MyTabWidget;
class TabBarButton;

namespace Andromeda {

class TabContainer : public GuiSystem::ProxyEditor
{
    Q_OBJECT
    Q_DISABLE_COPY(TabContainer)

public:
    explicit TabContainer(QWidget *parent = 0);

    void setSourceEditor(AbstractEditor *editor);

    int count() const;
    int currentIndex() const;

    QUrl defaultUrl() const;
    void setDefaultUrl(const QUrl &url);

    GuiSystem::IFile *file() const;
    GuiSystem::IHistory *history() const;

    MyTabWidget *tabWidget() const;

    bool restoreState(const QByteArray &state);
    QByteArray saveState() const;

public slots:
    void newTab();
    void newTab(const QUrl &url);
    void openEditor(const QList<QUrl> &urls, const QByteArray &editor);

    void closeTab(int index);
    void close();

    void setCurrentIndex(int index);

protected:
    void addTab(AbstractEditor *editor, bool changeTab = true);
    void resizeEvent(QResizeEvent *);

private slots:
    void onCurrentChanged(int);
    void onIconChanged(const QIcon &icon);
    void onTitleChanged(const QString &title);

private:
    AbstractEditor *createEditor();

private:
    MyTabWidget *m_tabWidget;
    TabBarButton *m_newTabButton;
    GuiSystem::ProxyHistory *m_proxyHistory;
    GuiSystem::ProxyFile *m_proxyFile;
    QUrl m_defaultUrl;
};

} // namespace Andromeda

#endif // TABCONTAINER_H
