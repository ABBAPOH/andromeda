#include <QtGui/QApplication>

#include <QtGui/QAction>
#include <GuiSystem/Command>
#include <GuiSystem/StandardCommands>
#include "mainwindow.h"

using namespace GuiSystem;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

#ifdef Q_OS_MAC
    app.setQuitOnLastWindowClosed(false);
    Command *quitCmd = StandardCommands::standardCommand(StandardCommands::Quit);
    quitCmd->setAttributes(Command::AttributeUpdateEnabled);
    app.connect(quitCmd->commandAction(), SIGNAL(triggered()), SLOT(quit()));
#endif

    MainWindow w;
    w.show();

    return app.exec();
}
