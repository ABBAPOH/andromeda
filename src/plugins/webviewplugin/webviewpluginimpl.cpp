#include "webviewpluginimpl.h"

#include <QtCore/QtPlugin>

#include <ieditorfactory.h>
#include "webvieweditor.h"

using namespace WebViewPlugin;
using namespace Core;

class WebViewFactory : public IEditorFactory
{
    virtual IEditor *open(const QString &path)
    {
        WebViewEditor *e = new WebViewEditor;
        e->open(path);
        return e;
    }
    virtual bool canOpen(const QString &path)
    {
        qDebug("canOpen");
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
    qDebug("WebViewPluginImpl::initialize");
    addObjectByType(new WebViewFactory, "EditorFactory");
    return true;
}

void WebViewPluginImpl::shutdown()
{
    qDebug("WebViewPluginImpl::shutdown");
}

Q_EXPORT_PLUGIN(WebViewPluginImpl)
