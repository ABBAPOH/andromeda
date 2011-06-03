#include "filecopydialog.h"

#include <QtGui/QScrollArea>
#include <QtGui/QVBoxLayout>
#include <QtGui/QResizeEvent>

#include "filecopywidget.h"

using namespace FileManagerPlugin;

namespace Ui {

class FileCopyReplaceDialog
{
public:
    QScrollArea *scrollArea;
    QVBoxLayout *layout;
    QWidget *widget;

    FileCopyReplaceDialog(){}

    void setupUi(QDialog *dialog)
    {
        dialog->resize(400, 400);
        layout = new QVBoxLayout;

        layout->addSpacerItem(new QSpacerItem(0,
                                              0,
                                              QSizePolicy::Preferred,
                                              QSizePolicy::Expanding)
                              ); // We need this spacer to have space in bottom of list

        widget = new QWidget;
        widget->setLayout(layout);

        scrollArea = new QScrollArea(dialog);
        scrollArea->setWidgetResizable(true);
        scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        scrollArea->setWidget(widget);
    }
};

} // namespace Ui

FileCopyDialog::FileCopyDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FileCopyReplaceDialog)
{
    ui->setupUi(this);
}

FileCopyDialog::~FileCopyDialog()
{
    delete ui;
}

void FileCopyDialog::addWidget(QWidget *widget)
{
    widget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    ui->layout->insertWidget(ui->layout->count() - 1, widget);
}

void FileCopyDialog::resizeEvent(QResizeEvent *e)
{
    ui->scrollArea->resize(e->size());
}
