#ifndef PERSPECTIVE_H
#define PERSPECTIVE_H

#include <QtCore/QObject>
#include <QtCore/QStringList>
#include <QtGui/QIcon>

#include "guisystem_global.h"

namespace GuiSystem {

class PerspectiveInstance;
class PerspectivePrivate;
class GUISYSTEM_EXPORT Perspective : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(Perspective)
public:
    explicit Perspective(QObject *parent = 0);
    explicit Perspective(const QString &id, QObject *parent = 0);
    Perspective(const QString &id, Perspective *parentPerspective);
    ~Perspective();

    QString id() const;
    void setId(const QString &id);

    QIcon icon() const;
    void setIcon(const QIcon &icon);

    QString name() const;
    void setName(const QString &name);

    Perspective *parentPerspective() const;

    void load(const QString &file);
    void save(const QString &file, const QByteArray format = QByteArray());

    void addView(const QString &id, int viewArea);
    void removeView(const QString &id);

    QStringList views() const;
    int viewArea(const QString &id) const;
    QVariant viewProperty(const QString &id, const QString &viewProperty) const;

protected:
    PerspectivePrivate *d_ptr;
    friend class PerspectiveInstance;
};

} // namespace GuiSystem

#endif // PERSPECTIVE_H
