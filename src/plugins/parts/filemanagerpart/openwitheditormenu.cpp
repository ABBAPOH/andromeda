#include "openwitheditormenu.h"

#include <Parts/AbstractDocumentFactory>
#include <Parts/DocumentManager>

using namespace Parts;

OpenWithEditorMenu::OpenWithEditorMenu(QWidget *parent) :
    QMenu(parent)
{
    setTitle(tr("Select editor"));

    connect(this, SIGNAL(triggered(QAction*)), SLOT(onTriggered(QAction*)));
}

QList<QUrl> OpenWithEditorMenu::urls() const
{
    return m_urls;
}

static void addAction(OpenWithEditorMenu *menu, AbstractDocumentFactory *factory, const QString &text = "%1")
{
    QAction *action = new QAction(menu);
    action->setText(text.arg(factory->name()));
    action->setIcon(factory->icon());
    action->setData(factory->id());
    menu->addAction(action);
}

void OpenWithEditorMenu::setUrls(const QList<QUrl> &urls)
{
    if (m_urls == urls)
        return;

    m_urls = urls;

    clear();

    DocumentManager *manager = DocumentManager::instance();
    QList<AbstractDocumentFactory *> factories = manager->factoriesForUrls(m_urls);
    if (factories.isEmpty())
        return;

    ::addAction(this, factories.first(), tr("%1 (default)"));

    if (factories.count() == 1)
        return;

    addSeparator();
    foreach (AbstractDocumentFactory *factory, factories.mid(1)) {
        ::addAction(this, factory);
    }
}

void OpenWithEditorMenu::onTriggered(QAction *action)
{
    QByteArray editor = action->data().toByteArray();

    emit openRequested(m_urls, editor);
}
