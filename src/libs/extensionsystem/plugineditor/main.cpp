#include <QtGui/QApplication>
#include <QtCore/QFile>

#include <ExtensionSystem/PluginSpec>
#include "mainwindow.h"

using namespace ExtensionSystem;

bool convert(const QString &source, const QString &dest)
{
    QFile f(source);
    if (!f.open(QIODevice::ReadOnly))
        return false;

    PluginSpec::Format format;
    if (f.peek(4) == "SPEC")
        format = PluginSpec::XmlFormat;
    else
        format = PluginSpec::BinaryFormat;

    PluginSpec spec;
    if (!spec.read(source))
        return false;

    if (!spec.write(dest, format))
        return false;

    return true;
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setOrganizationName("arch");
    app.setApplicationName("PluginEditor");

    if (argc == 3) {
        return !convert(QString::fromLocal8Bit(argv[1]), QString::fromLocal8Bit(argv[2]));
    }

    MainWindow w;
    w.show();

    if (argc == 2)
        w.open(QString::fromLocal8Bit(argv[1]));

    return app.exec();
}
