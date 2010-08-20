#ifndef ICORE_H
#define ICORE_H

#include "coreplugin_global.h"

#include <QtCore/QObject>

namespace RegistrationSystem
{
    class RegistrationManager;
}

using namespace RegistrationSystem;

namespace Core {

class COREPLUGIN_EXPORT ICore : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(ICore);

public:
    explicit ICore(QObject *parent = 0);

    static ICore *instance();

    virtual RegistrationManager *registrationManager() = 0;

signals:

public slots:

};

} // namespace Core

#endif // ICORE_H
