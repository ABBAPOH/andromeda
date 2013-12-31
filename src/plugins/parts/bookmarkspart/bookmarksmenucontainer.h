#ifndef BOOKMARKSMENUCONTAINER_H
#define BOOKMARKSMENUCONTAINER_H

#include <QtCore/QList>
#include <Parts/ApplicationCommand>
#include <Parts/ModelContainer>

namespace Bookmarks {

class BookmarksMenuContainer : public Parts::ModelContainer
{
    Q_OBJECT
public:
    explicit BookmarksMenuContainer(const QByteArray &id, QObject *parent = 0);

protected slots:
    void onModelReset();
    void onRowsInserted(const QModelIndex &parent, int first, int last);
    void onRowsRemoved(const QModelIndex &parent, int first, int last);

protected:
    explicit BookmarksMenuContainer(QObject *parent = 0);
    Parts::ModelContainer *createContainer();

private:
    void init();
    void updateActionsState();
    QList<QUrl> urls() const;

private slots:
    void openStrategy();

private:
    QList<Parts::ApplicationCommand *> commands;
};

} // namespace Bookmarks

#endif // BOOKMARKSMENUCONTAINER_H
