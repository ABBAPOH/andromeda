#ifndef TABCONTAINER_H
#define TABCONTAINER_H

#include <guisystem/abstractcontainer.h>
#include <guisystem/proxyhistory.h>

#include <QtCore/QPointer>

class MyTabWidget;
class TabBarButton;

namespace Core {

class TabContainer : public GuiSystem::AbstractContainer
{
    Q_OBJECT
    Q_DISABLE_COPY(TabContainer)

public:
    explicit TabContainer(QWidget *parent = 0);

    int count() const;
    int currentIndex() const;
    AbstractEditor *editor(int index) const;

    QUrl defaultUrl() const;
    void setDefaultUrl(const QUrl &url);

    GuiSystem::AbstractHistory *history() const;

    MyTabWidget *tabWidget() const;

    bool restoreState(const QByteArray &state);
    QByteArray saveState() const;

public slots:
    void openNewEditor(const QUrl &url);

    void closeEditor(int index);

    void setCurrentIndex(int index);

    void newTab();

protected:
    void resizeEvent(QResizeEvent *);

private slots:
    void onCurrentChanged(int);
    void onIconChanged(const QIcon &icon);
    void onTitleChanged(const QString &title);

private:
    AbstractEditor *createEditor();
    void setEditor(AbstractEditor *editor);
    void connectEditor(AbstractEditor *editor);
    void disconnectEditor(AbstractEditor *editor);
    void reconnectEditor(AbstractEditor *editor);

private:
    MyTabWidget *m_tabWidget;
    TabBarButton *m_newTabButton;
    QPointer<AbstractEditor> m_editor;
    GuiSystem::ProxyHistory *m_proxyHistory;
    QUrl m_defaultUrl;
};

} // namespace Core

#endif // TABCONTAINER_H
