#ifndef BROWSERTABWIDGET_H
#define BROWSERTABWIDGET_H

#include <QtCore/QPointer>

#include <Parts/AbstractEditor>
#include <Parts/EditorView>

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

    Parts::AbstractDocument *currentDocument() const;
    Parts::AbstractEditor *currentEditor() const;
    Parts::EditorView *currentView() const;

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
    void addView(Parts::EditorView *view, bool changeTab = true);

private slots:
    void onEditorChanged();
    void onCurrentChanged(int);
    void onIconChanged(const QIcon &icon);
    void onTitleChanged(const QString &title);

private:
    Parts::EditorView *createView();
    int indexOf(Parts::AbstractDocument *document);
    void setView(Parts::EditorView *view);
    void setDocument(Parts::AbstractDocument *document);

private:
    QPointer<Parts::EditorView> m_view;
    QPointer<Parts::AbstractDocument> m_document;
    TabBarButton *m_newTabButton;
    QUrl m_defaultUrl;
};

} // namespace Andromeda

#endif // BROWSERTABWIDGET_H
