#ifndef FILEMANAGERPLUGINIMPL_H
#define FILEMANAGERPLUGINIMPL_H

#include <iplugin.h>

//namespace FileManagerPlugin {

class FileManagerPluginImpl : public ExtensionSystem::IPlugin
{
    Q_OBJECT
public:
    explicit FileManagerPluginImpl(QObject *parent = 0);

    virtual bool initialize();

signals:

public slots:

};

//} // namespace FileManagerPlugin

#endif // FILEMANAGERPLUGINIMPL_H
