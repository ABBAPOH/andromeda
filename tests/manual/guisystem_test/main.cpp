#include <QtGui/QApplication>
#include <QDebug>

#include <GuiSystem>
#include <QTimer>

#include "testview.h"
#include "testview2.h"

using namespace GuiSystem;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    IViewFactory *factory = new TestViewFactory2;

    GuiController::instance()->addFactory(new TestViewFactory);
    GuiController::instance()->addFactory(factory);
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
//    view->deleteLater();

    PerspectiveSwitcher s;
    s.setState(w.currentState());
//    s.show();
    w.addToolBar(Qt::LeftToolBarArea, &s);

    w.show();


    QTimer tim;
    tim.setSingleShot(true);
    QObject::connect(&tim, SIGNAL(timeout()), factory, SLOT(deleteLater()));
    tim.start(5000);

    return app.exec();
}
