#include "perspectivemanager.h"

#include <QtCore/QMap>

namespace CorePlugin {

class PerspectiveManagerPrivate
{
public:
    QMap<QString, QString> mapToPerspective;
};

} // namespace CorePlugin

using namespace CorePlugin;

PerspectiveManager::PerspectiveManager(QObject *parent) :
    QObject(parent),
    d_ptr(new PerspectiveManagerPrivate)
{
    setObjectName("perspectiveManager");
}

PerspectiveManager::~PerspectiveManager()
{
    delete d_ptr;
}

void PerspectiveManager::addPerspective(const QString &mimeType, const QString &perspectiveId)
{
    Q_D(PerspectiveManager);

    d->mapToPerspective.insert(mimeType, perspectiveId);
}

void PerspectiveManager::removePerspective(const QString &mimeType)
{
    Q_D(PerspectiveManager);

    d->mapToPerspective.value(mimeType);
}

QString PerspectiveManager::perspective(const QString &mimeType) const
{
    Q_D(const PerspectiveManager);

    return d->mapToPerspective.value(mimeType);
}
