#include <QtGui/QApplication>
#include <QDebug>

#include <GuiSystem>
#include <QTimer>
#include <QMenu>

#include "testview.h"
#include "testview2.h"
#include "test.h"

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

    ActionManager *manager = ActionManager::instance();
    manager->addMenu(new QMenu("File"));
    QMenu *menu = new QMenu("Edit");
    QAction *a = new QAction("act", manager);
    manager->registerAction(a, "iiiiiid!!!!!");
    manager->registerMenu(menu, "menuId");

    MainWindow w;

    w.currentState()->setCurrentPerspective("Test Perspective");
    IView * view = w.currentState()->currentInstance()->view("TestViewFactory");
    qDebug() << "found view" << view->metaObject()->className();
//    view->deleteLater();

    manager->addAction(a);
    manager->addMenu(menu);

    PerspectiveSwitcher s;
    s.setState(w.currentState());
//    s.show();
    w.addToolBar(Qt::LeftToolBarArea, &s);

    w.show();
    w.addState();

    Test t;
    QTimer tim;
    tim.setSingleShot(true);
    QObject::connect(&tim, SIGNAL(timeout()), &t, SLOT(testDeletion()));
//    tim.start(5000);

    return app.exec();
}
