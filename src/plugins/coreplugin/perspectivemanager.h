#ifndef PERSPECTIVEMANAGER_H
#define PERSPECTIVEMANAGER_H

#include <QtCore/QObject>

namespace CorePlugin {

class PerspectiveManagerPrivate;
class PerspectiveManager : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(PerspectiveManager)
public:
    explicit PerspectiveManager(QObject *parent = 0);
    ~PerspectiveManager();

    // holywar here
    void addPerspective(const QString &mimeType, const QString &perspectiveId);
    void removePerspective(const QString &mimeType);

    QString perspective(const QString &mimeType) const;

signals:

public slots:

protected:
    PerspectiveManagerPrivate *d_ptr;
};

} // namespace CorePlugin

#endif // PERSPECTIVEMANAGER_H
