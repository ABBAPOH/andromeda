#include "webviewpluginimpl.h"

#include <QtCore/QtPlugin>

#include <ieditorfactory.h>
#include "webvieweditor.h"

using namespace WebViewPlugin;
using namespace Core;

class WebViewFactory : public IEditorFactory
{
    virtual IEditor *createEditor()
    {
        return new WebViewEditor;
    }
    virtual bool canOpen(const QString &path)
    {
        return path.startsWith("http://");
    }
    virtual QString type()
    {
        return "web";
    }

};

WebViewPluginImpl::WebViewPluginImpl() :
    ExtensionSystem::IPlugin()
{
}

bool WebViewPluginImpl::initialize()
{
    addObjectByType(new WebViewFactory, "EditorFactory");
    return true;
}

void WebViewPluginImpl::shutdown()
{
}

Q_EXPORT_PLUGIN(WebViewPluginImpl)
