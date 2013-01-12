#include "application.h"

#include <QtCore/QDataStream>
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QSettings>
#include <QtCore/QUrl>

#include <QtGui/QDesktopServices>
#include <QtGui/QFileOpenEvent>

#include <ImageView/ImageViewSettings>

#include "mainwindow.h"

using namespace ImageViewer;

static const quint32 m_magic = 0x6130396e; // "a09n"
static const quint8 m_version = 1;

Application::Application(const QString &id, int &argc, char **argv) :
    QtSingleApplication(id, argc, argv)
{
    setApplicationName("ImageViewer");
    setOrganizationName("arch");
    connect(this, SIGNAL(messageReceived(QString)), SLOT(handleMessage(QString)));
    connect(this, SIGNAL(aboutToQuit()), SLOT(onAboutToQuit()));
}

QByteArray Application::saveSession() const
{
    QByteArray result;
    QDataStream s(&result, QIODevice::WriteOnly);
    QList<QByteArray> states;
    s << m_magic;
    s << m_version;
    foreach (MainWindow *window, MainWindow::windows()) {
        states.append(window->saveState());
    }

    s << states;
    return result;
}

bool Application::restoreSession(const QByteArray &arr)
{
    QByteArray state(arr);
    QDataStream s(&state, QIODevice::ReadOnly);
    QList<QByteArray> states;
    quint32 magic;
    quint8 version;

    s >> magic;
    if (magic != m_magic)
        return false;

    s >> version;
    if (version != m_version)
        return false;

    s >> states;
    foreach (const QByteArray &state, states) {
        MainWindow *window = new MainWindow;
        window->setAttribute(Qt::WA_DeleteOnClose);
        window->restoreState(state);
        window->show();
    }

    return true;
}

void Application::handleArguments(const QStringList &arguments)
{
    if (arguments.isEmpty())
        return;

    QDir appDir(arguments.first());
    QStringList files;
    foreach (const QString &argument, arguments.mid(1)) {
        QUrl url;
        if (argument.startsWith("file://")) {
            url = QUrl(argument);
        } else {
            QFileInfo info(argument);
            if (info.isAbsolute())
                url = QUrl::fromLocalFile(info.absoluteFilePath());
            else {
                url = QUrl::fromLocalFile(appDir.absoluteFilePath(argument));
            }
        }

        if (url.isLocalFile()) {
            QString file = url.toLocalFile();
            if (!file.isEmpty())
                files.append(file);
        }
    }

    if (!files.isEmpty()) {
        MainWindow::openWindow(files);
        if (!topLevelWidgets().isEmpty())
            return;
    }

    if (topLevelWidgets().isEmpty())
        MainWindow::newWindow();
}

bool Application::restoreSession()
{
    QString dataPath = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
    QString filePath = dataPath + QLatin1Char('/') + QLatin1String("session");

    QFile f(filePath);
    bool ok = true;
    if (ok)
        ok = f.open(QFile::ReadOnly);

    if (ok) {
        QByteArray state = f.readAll();
        ok = restoreSession(state);
        if (!ok)
            qWarning() << tr("Couldn't restore session (located at %1)").arg(filePath);
    }

    return ok;
}

void Application::handleMessage(const QString &message)
{
    QStringList arguments = message.split('\n');
    handleArguments(arguments);
}

void Application::onAboutToQuit()
{
    saveSettings();
    storeSession();
}

bool Application::notify(QObject *object, QEvent *event)
{
    if (event->type() == QEvent::FileOpen) {
        QFileOpenEvent *foe = static_cast<QFileOpenEvent*>(event);
        MainWindow::openWindow(foe->url().toLocalFile());
        return true;
    }

    return QApplication::notify(object, event);
}

void Application::loadSettings()
{
    QSettings settings;
    settings.beginGroup("Image Viewer");
    ImageViewSettings *imageSettings = ImageViewSettings::globalSettings();

    int type = settings.value("image background type", ImageViewSettings::None).toInt();
    QColor imageColor = settings.value("image background color", QColor(255, 255, 255)).value<QColor>();
    QColor backgroundColor = settings.value("background color", QColor(128, 128, 128)).value<QColor>();
    bool useOpenGL = settings.value("use OpenGL", false).toBool();

    imageSettings->setiImageBackgroundType(ImageViewSettings::ImageBackgroundType(type));
    imageSettings->setImageBackgroundColor(imageColor);
    imageSettings->setBackgroundColor(backgroundColor);
    imageSettings->setUseOpenGL(useOpenGL);
}

void Application::saveSettings()
{
    QSettings settings;
    settings.beginGroup("Image Viewer");
    ImageViewSettings *imageSettings = ImageViewSettings::globalSettings();

    settings.setValue("image background type", (int)imageSettings->imageBackgroundType());
    settings.setValue("image background color", imageSettings->imageBackgroundColor());
    settings.setValue("background color", imageSettings->backgroundColor());
    settings.setValue("use OpenGL", imageSettings->useOpenGL());
}

void Application::storeSession()
{
    QString dataPath = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
    QDir().mkpath(dataPath);
    QString filePath = dataPath + QLatin1Char('/') + QLatin1String("session");
    QFile f(filePath);

    if (!f.open(QFile::WriteOnly))
        return;

    f.write(saveSession());
}
