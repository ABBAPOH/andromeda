#include "imageviewplugin.h"
#include "imagevieweditor.h"

#include <QtCore/QtPlugin>
#include <guisystem/editormanager.h>

using namespace GuiSystem;
using namespace ImageView;

ImageViewPlugin::ImageViewPlugin() :
    ExtensionSystem::IPlugin()
{
}

bool ImageViewPlugin::initialize()
{
    ImageViewEditorFactory *f = new ImageViewEditorFactory(this);
    EditorManager::instance()->addFactory(f);

    return true;
}

Q_EXPORT_PLUGIN(ImageViewPlugin)
