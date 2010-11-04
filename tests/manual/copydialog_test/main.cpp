#include <QtGui/QApplication>
#include <QDebug>

#include <filecopymanager.h>
#include <QtFileCopier>
#include <QDir>

using namespace IO;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    FileCopyManager *manager = FileCopyManager::instance();
    QtFileCopier * copier = manager->copier();
    qDebug() << "mkdir" << QDir("/Users/arch").mkdir("1");
    copier->copyDirectory("/Users/arch/Sites", "/Users/arch/1", QtFileCopier::NonInteractive);

    QObject::connect(copier, SIGNAL(done(bool)), &app, SLOT(quit()));

    return app.exec();
}
