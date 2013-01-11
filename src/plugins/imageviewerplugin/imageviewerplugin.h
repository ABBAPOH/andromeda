#ifndef IMAGEVIEWERPLUGIN_H
#define IMAGEVIEWERPLUGIN_H

#include <ExtensionSystem/IPlugin>

class QByteArray;
class QKeySequence;

namespace GuiSystem {
class CommandContainer;
} // namespace GuiSystem

namespace ImageViewer {

class ImageViewerPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT

public:
    explicit ImageViewerPlugin();

    bool initialize();

private:
    void createActions();
    void createAction(const QByteArray &id, const QString &text, const QKeySequence &shortcut);

private:
    GuiSystem::CommandContainer *imageViewMenu;
};

} // namespace ImageViewer

#endif // IMAGEVIEWERPLUGIN_H
