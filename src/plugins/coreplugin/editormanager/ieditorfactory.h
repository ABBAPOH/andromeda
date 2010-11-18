#ifndef IEDITORFACTORY_H
#define IEDITORFACTORY_H

#include "../coreplugin_global.h"

#include <QtCore/QObject>
#include "ieditor.h"

namespace Core
{

class COREPLUGIN_EXPORT IEditorFactory : public QObject
{
    Q_OBJECT
public:
    explicit IEditorFactory(QObject *parent = 0);

    virtual IEditor *open(const QUrl &url) = 0;
    virtual bool canOpen(const QUrl &url) = 0;
    virtual QString type() = 0;

    // used for stacking:
//    virtual bool allowStacking() { return false; }
//    virtual int priority() { return 0; }
    virtual QString group() { return QString::null; }
};

} // namespace Core

#endif // IEDITORFACTORY_H
