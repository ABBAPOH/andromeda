#include <QtGui/QApplication>
#include <QDebug>

#include <GuiSystem>

#include "testview.h"
#include "testview2.h"

using namespace GuiSystem;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    GuiController::instance()->addFactory(new TestViewFactory);
    GuiController::instance()->addFactory(new TestViewFactory2);
    Perspective *perspective = new Perspective("Test Perspective");
    perspective->setName("Perspective");

    perspective->addView("TestViewFactory", MainWindow::LeftViewArea);
    perspective->addView("TestViewFactory2", MainWindow::CentralViewArea);

    GuiController::instance()->addPerspective(perspective);

    perspective = new Perspective("Test Perspective2");
    perspective->setName("Perspective 2");
    perspective->addView("TestViewFactory2", MainWindow::CentralViewArea);

    GuiController::instance()->addPerspective(perspective);

    MainWindow w;
    w.currentState()->setCurrentPerspective("Test Perspective");
    IView * view = w.currentState()->currentInstance()->view("TestViewFactory");
    qDebug() << "found view" << view->metaObject()->className();
    view->deleteLater();

    PerspectiveSwitcher s;
    s.setState(w.currentState());
//    s.show();
    w.addToolBar(Qt::LeftToolBarArea, &s);

    w.show();

    return app.exec();
}
