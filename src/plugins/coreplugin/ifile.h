#ifndef IFILE_H
#define IFILE_H

#include <QtCore/QObject>

class IFile : public QObject
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

#endif // IFILE_H
