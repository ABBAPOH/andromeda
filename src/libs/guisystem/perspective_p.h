#ifndef PERSPECTIVE_P_H
#define PERSPECTIVE_P_H

#include "perspective.h"

#include <QtCore/QMap>

namespace GuiSystem {

class PerspectivePrivate
{
public:
    QString id;
    QString name;
    QStringList types;

    QMap<QString, ViewOptions> views;
};

} // namespace GuiSystem

#endif // PERSPECTIVE_P_H
