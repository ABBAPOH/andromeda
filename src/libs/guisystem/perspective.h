#ifndef PERSPECTIVE_H
#define PERSPECTIVE_H

#include <QtCore/QObject>
#include <QtCore/QStringList>

#include "guisystem_global.h"

#include "viewoptions.h"

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
    Perspective(const QString &id, const QStringList &types, QObject *parent = 0);
    ~Perspective();

    QString id() const;
    void setId(const QString &id);

    QString name() const;
    void setName(const QString &name);

    QStringList types() const;
    void setTypes(const QStringList &types);
    void addType(const QString &type);

    void load(const QString &file);
    void save(const QString &file, const QByteArray format = QByteArray());

    void addView(const ViewOptions &options);
    void addView(const QString &id, int area, int width = 0, int height = 0);
    QStringList views() const;
    ViewOptions viewOptions(const QString &id) const;

signals:

public slots:

protected:
    PerspectivePrivate *d_ptr;
    friend class PerspectiveInstance;
};

} // namespace GuiSystem

#endif // PERSPECTIVE_H
