#ifndef IOHANDLER_H
#define IOHANDLER_H

#include "guisystem_global.h"

#include <QtCore/QString>

namespace GuiSystem {

class Perspective;

class GUISYSTEM_EXPORT IIOHandler
{
public:
    IIOHandler() {}
    virtual ~IIOHandler() {}

    virtual bool canHandle(const QString &) = 0;
    virtual bool canWrite() { return false; }

    virtual QString description() { return ""; }

    virtual QByteArray format() = 0;

    virtual bool read(const QString &, Perspective *) = 0;
    virtual bool write(const QString &, const Perspective *) { return false; }
};

} // namespace GuiSystem

#endif // IOHANDLER_H
