#include <QDir>
#include <QLocale>
#include <QUrl>
#include <QTranslator>
#include <QMessageBox>

#include "application.h"
#include "mainwindow.h"

static QString getRootPath()
{
    // Figure out root:  Up one from 'bin' or 'MacOs'
    QDir rootDir = QCoreApplication::applicationDirPath();
    rootDir.cdUp();
    return rootDir.canonicalPath();
}

static inline QString getDefaultTranslationsPath()
{
    const QString rootDirPath = getRootPath();
    // Build path
    QString result = rootDirPath;
#if defined Q_OS_MACX
    result += QLatin1Char('/');
    result += QLatin1String("Resources");
    result += QLatin1Char('/');
    result += QLatin1String("Translations");
#elif defined Q_OS_WIN
    result += QLatin1Char('/');
    result += QLatin1String("translations");
#elif defined Q_OS_UNIX
    // not Mac UNIXes
    result += QLatin1Char('/');
    result += QLatin1String("share");
    result += QLatin1Char('/');
    result += qApp->applicationName();
    result += QLatin1Char('/');
    result += QLatin1String("translations");
#endif
    return result;
}

int main(int argc, char *argv[])
{
    Application app("QImageViewer", argc, argv);

    QStringList arguments = app.arguments();
    arguments[0] = QDir::currentPath();

    if (app.isRunning()) {
        app.sendMessage(arguments.join("\n"));
        return 0;
    }

    QString translationsPath = getDefaultTranslationsPath();
    QTranslator qtTranslator;
    qtTranslator.load("qt_" + QLocale::system().name(),
                      translationsPath);
    app.installTranslator(&qtTranslator);

    QTranslator translator;
    translator.load("qimageviewer_" + QLocale::system().name(), translationsPath);
    app.installTranslator(&translator);

    app.loadSettings();
    app.restoreSession();

    app.handleArguments(arguments);

    return app.exec();
}
