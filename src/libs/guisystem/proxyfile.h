#ifndef PROXYFILE_H
#define PROXYFILE_H

#include "guisystem_global.h"

#include "ifile.h"

namespace GuiSystem {

class GUISYSTEM_EXPORT ProxyFile : public IFile
{
    Q_OBJECT
    Q_DISABLE_COPY(ProxyFile)

public:
    explicit ProxyFile(QObject *parent = 0);

    IFile *sourceFile() const;
    void setSourceFile(IFile *file);

    bool isModified() const;
    bool isReadOnly() const;

public slots:
    void setModified(bool modified = true);
    void setReadOnly(bool readOnly = true);

    void save(const QUrl &url = QUrl());

private:
    IFile *m_sourceFile;
};

} // namespace GuiSystem

#endif // PROXYFILE_H
