#ifndef FILEDOCUMENT_H
#define FILEDOCUMENT_H

#include "../guisystem_global.h"

#include <GuiSystem/AbstractDocument>

namespace GuiSystem {

class FileDocumentPrivate;
class GUISYSTEM_EXPORT FileDocument : public AbstractDocument
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(FileDocument)
    Q_DISABLE_COPY(FileDocument)
public:
    explicit FileDocument(QObject *parent = 0);
    ~FileDocument();

public slots:
    void clear();
    void save(const QUrl &url);

protected slots:
    void onReadyRead();
    void onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void onReplyFinished();

protected:
    FileDocument(FileDocumentPrivate &dd, QObject *parent);

    bool openUrl(const QUrl &url);

    virtual bool read(QIODevice *device, const QString &fileName) = 0;
    virtual bool write(QIODevice *device, const QString &fileName);
};

} // namespace GuiSystem

#endif // FILEDOCUMENT_H
