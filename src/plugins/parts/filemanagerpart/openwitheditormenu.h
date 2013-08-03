#ifndef OPENWITHEDITORMENU_H
#define OPENWITHEDITORMENU_H

#include <QtCore/QUrl>

#if QT_VERSION >= 0x050000
#include <QtWidgets/QMenu>
#else
#include <QtGui/QMenu>
#endif

class OpenWithEditorMenu : public QMenu
{
    Q_OBJECT
    Q_PROPERTY(QList<QUrl> urls READ urls WRITE setUrls NOTIFY urlsChanged)

public:
    explicit OpenWithEditorMenu(QWidget *parent = 0);

    QList<QUrl> urls() const;
    void setUrls(const QList<QUrl> &urls);

signals:
    void urlsChanged(const QList<QUrl> &urls);
    void openRequested(const QList<QUrl> &urls, const QByteArray &editor);

private slots:
    void onTriggered(QAction *action);

private:
    QList<QUrl> m_urls;
};

#endif // OPENWITHEDITORMENU_H
