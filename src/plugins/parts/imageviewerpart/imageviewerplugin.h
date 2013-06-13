#ifndef IMAGEVIEWERPLUGIN_H
#define IMAGEVIEWERPLUGIN_H

#include <ExtensionSystem/IPlugin>

class QByteArray;
class QKeySequence;

namespace Parts {
class CommandContainer;
} // namespace Parts

namespace ImageViewer {

class ImageViewerPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "com.arch.Andromeda.ImageViewerPlugin")
#endif
public:
    explicit ImageViewerPlugin();

    bool initialize();

private:
    void createActions();
    void createAction(const QByteArray &id, const QString &text, const QKeySequence &shortcut);

private:
    Parts::CommandContainer *imageViewMenu;
};

} // namespace ImageViewer

#endif // IMAGEVIEWERPLUGIN_H
