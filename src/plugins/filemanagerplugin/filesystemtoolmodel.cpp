#include "filesystemtoolmodel.h"

#include <GuiSystem/AbstractDocument>
#include <FileManager/FileSystemModel>

using namespace GuiSystem;
using namespace FileManager;

FileSystemToolModel::FileSystemToolModel(QObject *parent) :
    ToolModel(parent),
    m_model(new FileSystemModel(this))
{
    m_model->setRootPath(QDir::rootPath());
    m_model->setReadOnly(false);
    setTitle(tr("File system"));
}

FileSystemModel * FileSystemToolModel::fileSystemModel() const
{
    return m_model;
}

QUrl FileSystemToolModel::url() const
{
    if (!document())
        return QUrl();

    return document()->url();
}

void FileSystemToolModel::setDocument(AbstractDocument *document)
{
    AbstractDocument *oldDocument = this->document();
    if (oldDocument == document)
        return;

    if (oldDocument)
        disconnect(oldDocument, SIGNAL(urlChanged(QUrl)), this, SIGNAL(urlChanged(QUrl)));

    ToolModel::setDocument(document);

    if (document)
        connect(document, SIGNAL(urlChanged(QUrl)), this, SIGNAL(urlChanged(QUrl)));
//    emit urlChanged(document ? document->url() : QUrl());
}
