#include <QtGui/QApplication>
#include <QDebug>

#include "test.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    Test t;

    return app.exec();
}
