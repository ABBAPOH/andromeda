#include "bookmarksmenucontainer.h"

#include <Bookmarks/BookmarksModel>
#include <Parts/OpenStrategy>

using namespace Bookmarks;
using namespace Parts;

BookmarksMenuContainer::BookmarksMenuContainer(const QByteArray &id, QObject *parent) :
    ModelContainer(id, parent)
{
    init();
}

BookmarksMenuContainer::BookmarksMenuContainer(QObject *parent) :
    ModelContainer(parent)
{
    init();
}

void BookmarksMenuContainer::onModelReset()
{
    ModelContainer::onModelReset();
    updateActionsState();
}

void BookmarksMenuContainer::onRowsInserted(const QModelIndex &parent, int first, int last)
{
    ModelContainer::onRowsInserted(parent, first, last);
    updateActionsState();
}

void BookmarksMenuContainer::onRowsRemoved(const QModelIndex &parent, int first, int last)
{
    ModelContainer::onRowsRemoved(parent, first, last);
    updateActionsState();
}

ModelContainer *BookmarksMenuContainer::createContainer()
{
    return new BookmarksMenuContainer(this);
}

void BookmarksMenuContainer::init()
{
    foreach (OpenStrategy *strategy, OpenStrategy::strategies()) {
        ApplicationCommand *c = new ApplicationCommand(this);
        c->setToolTip(strategy->toolTip());
        c->setText(strategy->text());
        c->setData(strategy->id());
        connect(c, SIGNAL(triggered()), this, SLOT(openStrategy()));
        commands.append(c);
        addCommand(c);
    }
}

void BookmarksMenuContainer::updateActionsState()
{
    const QList<QUrl> urls = this->urls();
    foreach (ApplicationCommand *c, commands) {
        const QByteArray id = c->data().toByteArray();
        OpenStrategy *strategy = OpenStrategy::strategy(id);
        if (!strategy)
            continue;
        c->setEnabled(strategy->canOpen(urls));
    }
}

QList<QUrl> BookmarksMenuContainer::urls() const
{
    QList<QUrl> urls;
    const QModelIndex parent = rootIndex();
    for (int i = 0; i < model()->rowCount(parent); ++i) {
        QModelIndex index = model()->index(i, 0, parent);
        if (model()->hasChildren(index))
            continue;
        urls.append(index.data(BookmarksModel::UrlRole).toUrl());
    }
    return urls;
}

void BookmarksMenuContainer::openStrategy()
{
    ApplicationCommand *command = qobject_cast<ApplicationCommand *>(sender());
    if (!command)
        return;

    const QByteArray id = command->data().toByteArray();
    OpenStrategy *strategy = OpenStrategy::strategy(id);
    if (!strategy)
        return;
    strategy->open(urls());
}
