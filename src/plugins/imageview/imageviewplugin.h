#ifndef HELLOWORLPLUGIN_H
#define HELLOWORLPLUGIN_H

#include <extensionsystem/iplugin.h>

class QByteArray;
class QKeySequence;

namespace GuiSystem {
class CommandContainer;
} // namespace GuiSystem

class ImageViewPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT

public:
    explicit ImageViewPlugin();

    bool initialize();

private:
    void createActions();
    void createAction(const QByteArray &id, const QString &text, const QKeySequence &shortcut);

private:
    GuiSystem::CommandContainer *imageViewMenu;
};

#endif // HELLOWORLPLUGIN_H
