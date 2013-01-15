#include "imageviewerplugin.h"

#include <QtCore/QtPlugin>
#include <GuiSystem/ActionManager>
#include <GuiSystem/DocumentManager>
#include <GuiSystem/EditorManager>
#include <GuiSystem/MenuBarContainer>

#include "imageviewerdocument.h"
#include "imageviewereditor.h"

using namespace GuiSystem;
using namespace ImageViewer;

ImageViewerPlugin::ImageViewerPlugin() :
    ExtensionSystem::IPlugin()
{
}

bool ImageViewerPlugin::initialize()
{
    DocumentManager::instance()->addFactory(new ImageViewerDocumentFactory(this));
    EditorManager::instance()->addFactory(new ImageViewerEditorFactory(this));

    createActions();

    return true;
}

void ImageViewerPlugin::createActions()
{
    MenuBarContainer *menuBar = MenuBarContainer::instance();
    CommandContainer *toolsMenu = menuBar->container(MenuBarContainer::ToolsMenu);

    imageViewMenu = new CommandContainer("ImageViewMenu", this);
    imageViewMenu->setTitle(tr("Image view"));
    toolsMenu->addContainer(imageViewMenu);

    createAction("MoveTool", tr("Move tool"), QKeySequence("Ctrl+1"));
    createAction("SelectionTool", tr("Selection tool"), QKeySequence("Ctrl+2"));

    imageViewMenu->addCommand(new Separator(this));
    createAction("ZoomIn", tr("Zoom in"), QKeySequence("Ctrl+="));
    createAction("ZoomOut", tr("Zoom out"), QKeySequence("Ctrl+-"));
    createAction("FitInView", tr("Fit in view"), QKeySequence("Ctrl+9"));
    createAction("NormalSize", tr("Normal size"), QKeySequence("Ctrl+0"));

    imageViewMenu->addCommand(new Separator(this));
    createAction("RotateLeft", tr("Rotate left"), QKeySequence("Ctrl+L"));
    createAction("RotateRight", tr("Rotate right"), QKeySequence("Ctrl+R"));
    createAction("FlipHorizontally", tr("Flip horizontally"), QKeySequence("Ctrl+Shift+H"));
    createAction("FlipVertically", tr("Flip vertically"), QKeySequence("Ctrl+Shift+V"));
    createAction("ResetOriginal", tr("Reset original"), QKeySequence());
}

void ImageViewerPlugin::createAction(const QByteArray &id, const QString &text, const QKeySequence &shortcut)
{
    Command *c = new Command(id, this);
    c->setDefaultText(text);
    c->setDefaultShortcut(shortcut);
    imageViewMenu->addCommand(c);
}

Q_EXPORT_PLUGIN(ImageViewerPlugin)
