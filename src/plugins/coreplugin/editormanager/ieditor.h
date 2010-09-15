#ifndef IEDITOR_H
#define IEDITOR_H

#include "../coreplugin_global.h"

#include <QtCore/QObject>
#include <QtCore/QString>

namespace Core
{

class COREPLUGIN_EXPORT IEditor : public QObject
{
    Q_OBJECT
public:
    explicit IEditor(QObject *parent = 0);

    virtual bool open(const QString &file) = 0;
    virtual QWidget *widget() = 0;

signals:
    void changed();

public slots:

};

} // namespace Core

#endif // IEDITOR_H
