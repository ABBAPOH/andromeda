#include "filedocument.h"
#include "filedocument_p.h"

#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QTemporaryFile>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QDebug>

using namespace GuiSystem;

/*!
    \class GuiSystem::FileDocument

    \brief FileDocument is a base class for file-based documents.

    Typically, file base document is a byte array on a disk. FileDocument class
    allows to simplify reading and writing files and also adds network
    transparency.
*/

/*!
    Constructs an FileDocument with the given \a parent.
*/
FileDocument::FileDocument(QObject *parent) :
    AbstractDocument(*new FileDocumentPrivate(this), parent)
{
}

/*!
    Constructs an FileDocumentPrivate with the given \a parent and private class \a dd.
*/
FileDocument::FileDocument(FileDocumentPrivate &dd, QObject *parent) :
    AbstractDocument(dd, parent)
{
}

/*!
    Destroys an AbstractDocument.
*/
FileDocument::~FileDocument()
{
}

void FileDocument::clear()
{
    Q_D(FileDocument);
    delete d->file;
    d->file = 0;
}

/*!
    \reimp
*/
void FileDocument::save(const QUrl &url)
{
    if (url.isEmpty() && isReadOnly())
        return;

    if (url.scheme() != "file")
        return;

    Q_D(FileDocument);

    QString filePath = url.toLocalFile();
    QFile *file = new QFile(filePath, this);
    if (!file->open(QFile::WriteOnly))
        return;

    /*bool ok = */write(file, QFileInfo(url.path()).fileName());
    file->close();
    delete d->file;
    d->file = file;

    setReadOnly(false);
}

/*!
    \internal
*/
void FileDocument::onReadyRead()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());

    Q_D(FileDocument);
    qint64 bytes = reply->bytesAvailable();
    d->file->write(reply->read(bytes));
}

/*!
    \internal
*/
void FileDocument::onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    int progress = (1.0*bytesReceived/bytesTotal)*100.0;
    setProgress(progress);
}

/*!
    \internal
*/
void FileDocument::onReplyFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    reply->deleteLater();
    reply->manager()->deleteLater();

    Q_D(FileDocument);
    d->file->close();
    setState(NoState);
    d->file->open(QFile::ReadOnly);

    read(d->file, QFileInfo(url().path()).fileName());
}

/*!
    \reimp
*/
bool FileDocument::openUrl(const QUrl &url)
{
    Q_D(FileDocument);

    if (d->file)
        delete d->file;

    if (url.scheme() == "file") {
        QString filePath = url.toLocalFile();
        d->file = new QFile(filePath, this);
        if (!d->file->open(QFile::ReadOnly))
            return false;
        if (!d->file)
            return false;

        setReadOnly(false);
        return read(d->file, QFileInfo(url.path()).fileName());
    } else if (url.scheme() == "http") {
        setReadOnly(true);
        setState(OpenningState);
        QNetworkAccessManager *qNAM = new QNetworkAccessManager(this);
        QNetworkReply *reply = qNAM->get(QNetworkRequest(url));
        connect(reply, SIGNAL(readyRead()), SLOT(onReadyRead()));
        connect(reply, SIGNAL(downloadProgress(qint64,qint64)),
                SLOT(onDownloadProgress(qint64,qint64)));
        connect(reply, SIGNAL(finished()), SLOT(onReplyFinished()));

        d->file = new QTemporaryFile(this);
        d->file->open(QFile::WriteOnly);
    }
    return true;
}

/*!
    \fn bool FileDocument::read(QIODevice *device, const QString &fileName)
    \brief Reipmlement this function to read data from the \a device.
*/

/*!
    \fn bool FileDocument::write(QIODevice *device, const QString &fileName)
    \brief Reipmlement this function to write data to the \a device.
*/
bool FileDocument::write(QIODevice *device, const QString &fileName)
{
    Q_UNUSED(device);
    Q_UNUSED(fileName);
    return false;
}
