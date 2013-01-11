#ifndef BROWSERTABWIDGET_H
#define BROWSERTABWIDGET_H

#include <QtCore/QPointer>

#include <GuiSystem/AbstractEditor>
#include <GuiSystem/EditorView>

#include <Widgets/TabWidget>

class TabBarButton;

namespace Andromeda {

class BrowserTabWidget : public TabWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(BrowserTabWidget)
public:
    explicit BrowserTabWidget(QWidget *parent = 0);
    ~BrowserTabWidget();

    QUrl defaultUrl() const;
    void setDefaultUrl(const QUrl &url);

    GuiSystem::AbstractDocument *currentDocument() const;
    GuiSystem::AbstractEditor *currentEditor() const;
    GuiSystem::EditorView *currentView() const;

    bool restoreState(const QByteArray &state);
    QByteArray saveState() const;

    int indexOf(QWidget *widget);

signals:
    void editorChanged();
    void viewChanged();

public slots:
    void newTab();
    void newTab(const QUrl &url);
    void open(const QUrl &url);
    void openEditor(const QList<QUrl> &urls, const QByteArray &editorId);

    void closeTab(int index);
    void close();

protected:
    void addView(GuiSystem::EditorView *view, bool changeTab = true);

private slots:
    void onEditorChanged();
    void onCurrentChanged(int);
    void onIconChanged(const QIcon &icon);
    void onTitleChanged(const QString &title);

private:
    GuiSystem::EditorView *createView();
    int indexOf(GuiSystem::AbstractDocument *document);
    void setView(GuiSystem::EditorView *view);
    void setDocument(GuiSystem::AbstractDocument *document);

private:
    QPointer<GuiSystem::EditorView> m_view;
    QPointer<GuiSystem::AbstractDocument> m_document;
    TabBarButton *m_newTabButton;
    QUrl m_defaultUrl;
};

} // namespace Andromeda

#endif // BROWSERTABWIDGET_H
