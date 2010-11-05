#include "filecopydialog.h"

#include "filecopywidget.h"
#include <QtGui/QScrollArea>
#include <QtGui/QVBoxLayout>
#include <QtGui/QResizeEvent>

using namespace MainWindowPlugin;

FileCopyDialog::FileCopyDialog(QWidget *parent) :
    QDialog(parent)
{
    setupUi();
}

void FileCopyDialog::addWidget(QWidget *widget)
{
    widget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    m_layout->insertWidget(m_layout->count() - 1, widget);
}

void FileCopyDialog::setupUi()
{
    resize(400, 400);
    m_layout = new QVBoxLayout;

    m_layout->addSpacerItem(new QSpacerItem(0,
                                            0,
                                            QSizePolicy::Preferred,
                                            QSizePolicy::Expanding)
                            ); // We need this spacer to have space in bottom of list

    m_widget = new QWidget;
    m_widget->setLayout(m_layout);

    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    m_scrollArea->setWidget(m_widget);
}

void FileCopyDialog::resizeEvent(QResizeEvent *e)
{
    m_scrollArea->resize(e->size());
}
