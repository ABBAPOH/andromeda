#ifndef IFILE_H
#define IFILE_H

#include "guisystem_global.h"

#include <QtCore/QObject>
#include <QtCore/QUrl>

namespace GuiSystem {

class GUISYSTEM_EXPORT IFile : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(IFile)

    Q_PROPERTY(bool modified READ isModified WRITE setModified NOTIFY modificationChanged)
    Q_PROPERTY(bool readOnly READ isReadOnly WRITE setReadOnly NOTIFY readOnlyChanged)

public:
    explicit IFile(QObject *parent = 0);

    virtual bool isModified() const;
    virtual bool isReadOnly() const;

public slots:
    virtual void setModified(bool modified = true);
    virtual void setReadOnly(bool readOnly = true);

    virtual void save(const QUrl &url = QUrl());

signals:
    void modificationChanged(bool modified);
    void readOnlyChanged(bool readOnly);
};

} // namespace GuiSystem

#endif // IFILE_H
