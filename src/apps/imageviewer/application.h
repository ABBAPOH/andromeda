#ifndef APPLICATION_H
#define APPLICATION_H

#include <qtsingleapplication.h>

namespace ImageViewer {

class Application : public QtSingleApplication
{
    Q_OBJECT
public:
    explicit Application(const QString &id, int &argc, char **argv);

    void handleArguments(const QStringList &arguments);

    bool restoreSession();
    void storeSession();

    void loadSettings();
    void saveSettings();

protected slots:
    void handleMessage(const QString &message);
    void onAboutToQuit();

protected:
    bool notify(QObject *object, QEvent *event);

    QByteArray saveSession() const;
    bool restoreSession(const QByteArray &state);
};

} // namespace ImageViewer

#endif // APPLICATION_H
