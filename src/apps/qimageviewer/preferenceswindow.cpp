#include "preferenceswindow.h"

#include <QtGui/QAction>
#include <QtGui/QVBoxLayout>

#include <qimageview/qimageviewpreferenceswidget.h>

PreferencesWindow::PreferencesWindow(QWidget *parent) :
    QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(new QImageViewPreferencesWidget(this));
    layout->setSizeConstraint(QLayout::SetFixedSize);

    QAction *closeAction = new QAction(this);
    closeAction->setShortcut(QKeySequence("Ctrl+W"));
    connect(closeAction, SIGNAL(triggered()), SLOT(close()));
    addAction(closeAction);

    setWindowIcon(QIcon(":/icons/qimageviewer.png"));
    setWindowTitle(tr("Preferences"));
}
