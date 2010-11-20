#include "webvieweditor.h"

#include <ifile.h>

using namespace WebViewPlugin;
using namespace Core;

class WebFile: public IFile
{
public:
    WebFile(WebViewEditor *editor) :
            m_editor(editor)
    {
    }
    virtual bool save(const QString &path)
    {}
    virtual void refresh()
    {}
    virtual QString path() const
    {
        return m_editor->m_webView->url().toString();
    }

    WebViewEditor *m_editor;
};

WebViewEditor::WebViewEditor(QObject *parent) :
    IEditor(parent)
{
    m_webView = new QWebView;
    m_file = new WebFile(this);
}
