#include "test.h"

#include <GuiSystem>

using namespace GuiSystem;

Test::Test(QObject *parent) :
    QObject(parent)
{
}

void Test::testDeletion()
{
    GuiController::instance()->removeFactory("TestViewFactory");
//    IViewFactory *factory = GuiController::instance()->factory("");
//    factory->deleteLater();
}
