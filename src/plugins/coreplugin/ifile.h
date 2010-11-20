#ifndef IFILE_H
#define IFILE_H

#include "coreplugin_global.h"

#include <QtCore/QObject>

class QString;
namespace Core {

class COREPLUGIN_EXPORT IFile : public QObject
{
    Q_OBJECT
public:
    explicit IFile(QObject *parent = 0);

    virtual bool save(const QString &path) = 0;

    virtual bool isModified() { return false; }
    virtual bool isReadOnly() { return true; }
//    virtual bool isDirectory(); // ??

    virtual QString name() { return ""; }
    virtual QString path() = 0;

signals:
    void changed();

public slots:

};

} // namespace Core

#endif // IFILE_H
