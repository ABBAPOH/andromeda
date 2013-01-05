#include "bineditordocument.h"

#include <QtCore/QFileInfo>
#include <QtGui/QFileIconProvider>

using namespace GuiSystem;
using namespace BINEditor;

BinEditorDocument::BinEditorDocument(QObject *parent) :
    AbstractDocument(parent)
{
}

bool BinEditorDocument::openUrl(const QUrl &url)
{
    QString localFile = url.toLocalFile();
    QFileInfo info = QFileInfo(localFile);

    setTitle(info.baseName());
    setIcon(QFileIconProvider().icon(info));

    return true;
}
