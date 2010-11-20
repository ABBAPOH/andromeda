#ifndef IEDITOR_H
#define IEDITOR_H

#include "../coreplugin_global.h"

#include <QtCore/QObject>
#include <QtCore/QString>

class QUrl;
class QWidget;
class QToolBar;
class IFile;

namespace Core
{

class COREPLUGIN_EXPORT IEditor : public QObject
{
    Q_OBJECT
public:
    explicit IEditor(QObject *parent = 0);

    virtual bool open(const QString &path) = 0;
    virtual bool create() = 0;

    virtual IFile *file() const = 0;
//    QList<IFile *> files();

    virtual QWidget *widget() const = 0;
    virtual QToolBar *toolBar() const = 0;

    virtual QString name() const = 0;

    virtual bool canClone() const { return false; }
    virtual IEditor *clone() const { return 0; }

//    virtual bool saveState() { return true; }
//    virtual bool restoreState() { return true; }

signals:
    void changed();

public slots:

};

} // namespace Core

#endif // IEDITOR_H
