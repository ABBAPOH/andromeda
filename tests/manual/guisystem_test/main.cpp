#include <QtGui/QApplication>
#include <QDebug>

#include <GuiSystem>

#include "test.h"

using namespace GuiSystem;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    Test t;

    return app.exec();
}
