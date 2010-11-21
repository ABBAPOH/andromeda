#ifndef IFILE_H
#define IFILE_H

#include "coreplugin_global.h"

#include <QtCore/QObject>
#include <QtGui/QIcon>

class QString;
namespace CorePlugin {

class COREPLUGIN_EXPORT IFile : public QObject
{
    Q_OBJECT
public:
    explicit IFile(QObject *parent = 0);

    virtual bool save(const QString &path) = 0;
    virtual void refresh() = 0;

    virtual bool isModified() const { return false; }
    virtual bool isReadOnly() const { return true; }
//    virtual bool isDirectory(); // ??

    virtual QIcon icon() const { return QIcon(); }
    virtual QString name() const { return ""; }
    virtual QString path() const = 0;

signals:
    void changed();

public slots:

};

} // namespace CorePlugin

#endif // IFILE_H
